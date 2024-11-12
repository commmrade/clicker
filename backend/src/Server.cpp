#include <cstdio>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <json/reader.h>
#include <json/value.h>
#include<klewy/Server.hpp>
#include <memory>
#include <utility>

#define DEBUG
#ifdef DEBUG
#define log(X) std::cout << X << std::endl
#endif

api::api() {
    db = std::make_unique<DatabaseHandler>("tcp://127.0.0.1:3306", "klewy", "root", "clicker");
}

void api::respond_error(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(code);
    callback(resp);
}

void api::respond_error(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback, Json::Value js) {
    auto resp = HttpResponse::newHttpJsonResponse(js);
    resp->setStatusCode(code);
    callback(resp);
}

bool api::check_auth(const HttpRequestPtr &request, const std::string &name) {
    auto resp = HttpResponse::newHttpResponse();
    std::string tkn = request->getHeader("Authorization");
    if (!SessionManager::check_key_and_name(tkn, name)) {
        return false;
    }
    return true;
}
void api::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    std::cout << req->body() << std::endl;
    auto json_obj = req->getJsonObject();
    
    if (!json_obj) {
        log("Empty why??");
        respond_error(HttpStatusCode::k400BadRequest, std::move(callback));
        
        return;
    }
   
    const std::string name = (*json_obj)["name"].asString();
    const std::string password = (*json_obj)["password"].asString();

    if (name.empty() || password.empty()) { //If data is not correct
        respond_error(HttpStatusCode::k400BadRequest, std::move(callback));
        return;
    }


    Json::Value js;
    
    auto passwd = db->get_password(name);
    if (passwd) { //User does exist
        if (bcrypt::validatePassword(password, passwd.value())) {
            js["token"] = SessionManager::make_key(name);
        } else {
            respond_error(drogon::k401Unauthorized, std::move(callback));
        }
    } else { 
        respond_error(k400BadRequest, std::move(callback));
    }

    HttpStatusCode code = drogon::k200OK;
    auto resp = HttpResponse::newHttpJsonResponse(js);
    resp->setStatusCode(code);
    callback(resp);
}
void api::login_token(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &token) {
    Json::Value js;
    if (token.empty()) {
        respond_error(HttpStatusCode::k401Unauthorized, std::move(callback));
        return;
    }

    if (!SessionManager::check_key(token)) {
        respond_error(k401Unauthorized, std::move(callback));
        return;
    }
    
    js["name"] = SessionManager::get_name(token);
    auto response = HttpResponse::newHttpJsonResponse(js);
    response->setStatusCode(HttpStatusCode::k200OK);
    
    callback(response);
}

void api::reg(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (json_obj->empty()) {
        respond_error(drogon::k400BadRequest, std::move(callback));
        return;
    }

    const std::string name = (*json_obj)["name"].asString();
    const std::string password = (*json_obj)["password"].asString();

    if (name.empty() || password.empty() || name.size() > 16 || password.size() > 32) { //if data is not correct
        respond_error(k400BadRequest, std::move(callback));
        return;
    }

    if(!db->add_user(name, bcrypt::generateHash(password))) {
        respond_error(k400BadRequest, std::move(callback));
        return;
    } 

    Json::Value js;
    HttpStatusCode code = drogon::k200OK;
    
    js["token"] = SessionManager::make_key(name);
    auto response = HttpResponse::newHttpJsonResponse(js);
    response->setStatusCode(code);

    callback(response);
}

void api::user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &name) {
    if (name.empty()) {
        respond_error(k400BadRequest, std::move(callback));
        return;
    }

    Json::Value js;
    js["balance"] = db->get_balance(name);
    js["click_mod"] = db->get_click_modifier(name);
    js["hourly_payout"] = db->get_per_hour_pay(name);
    js["hourly_payout_mod"] = db->get_pay_mod(name);
    js["last_pay"] = db->get_last_pay(name).value_or("NULL");
    js["click_mod_price"] = db->get_click_mod_price(name);
    js["hourly_payout_mod_price"] = db->get_pay_mod_price(name);

    auto response = HttpResponse::newHttpJsonResponse(js);
    response->setStatusCode(HttpStatusCode::k200OK);
    callback(response);
}

void api::clicks(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->jsonObject();
    

    if (!json_obj) {
        respond_error(drogon::k400BadRequest, std::move(callback));
        return;
    }
    const std::string name = (*json_obj)["name"].asString();
    const int click = std::stoi((*json_obj)["click"].asString());
    
    if (name.empty() || click == 0) {
        respond_error(k400BadRequest, std::move(callback));
        return;
    }

    if (!check_auth(req, name)) {
        respond_error(drogon::k401Unauthorized, std::move(callback));
        return;
    }

    db->add_clicks(name, click);
    auto resp = HttpResponse::newHttpResponse();
    callback(resp);
}

void api::purchase(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &name, const std::string &mod) {
    if (name.empty() || mod.empty()) {
        respond_error(k400BadRequest, std::move(callback));
        return;
    }
    
   
    if (!check_auth(req, name)) {
        respond_error(drogon::k401Unauthorized, std::move(callback));
        return;
    }
    auto resp = HttpResponse::newHttpResponse();
    double bal = db->get_balance(name);

    if (mod == "click") {
        double cur_mod_price = db->get_click_mod_price(name);
        if (bal < cur_mod_price) {
            respond_error(k400BadRequest, std::move(callback));
            return;
        }

        double cur_mod = db->get_click_modifier(name);
        
        db->set_click_mod(name, cur_mod * 1.1);
        db->set_click_mod_price(name, cur_mod_price * 1.2);
        db->set_balance(name, db->get_balance(name) - cur_mod_price);
        resp->setStatusCode(HttpStatusCode::k200OK);
    } else if (mod == "pay") {
        double cur_mod_price = db->get_pay_mod_price(name);
        if (bal < cur_mod_price) {
            respond_error(k400BadRequest, std::move(callback));
            return;
        }

        double cur_mod = db->get_pay_mod(name);

        db->set_pay_mod(name, cur_mod * 1.1);
        db->set_pay_mod_price(name, cur_mod_price * 1.2);
        db->set_balance(name, db->get_balance(name) - cur_mod_price);
        resp->setStatusCode(HttpStatusCode::k200OK);
    } 
    callback(resp);
}

void api::daily_pay(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (!json_obj) {
        respond_error(drogon::k400BadRequest, std::move(callback));
    }

    const std::string name = (*json_obj)["name"].asString();

    if (name.empty()) {
        respond_error(k400BadRequest, std::move(callback));
        return;
    }

    
    if (!check_auth(req, name)) {
        respond_error(k401Unauthorized, std::move(callback));
        return;
    }

    auto prev_time = db->get_last_pay(name);
    if (!prev_time.has_value()) {
        db->set_last_pay(name);
        respond_error(k400BadRequest, std::move(callback));
        return;
    }
    
    //Counting time
    std::tm tm{};
    std::stringstream ss{prev_time.value()};
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::time_t time = std::mktime(&tm);

    auto prev_time_chrono = std::chrono::system_clock::from_time_t(time);
    auto cur_time_chrono = std::chrono::system_clock::now();

    auto duration = cur_time_chrono - prev_time_chrono;
    auto hrs_passed = std::chrono::duration_cast<std::chrono::hours>(duration);

    double money_total = (hrs_passed.count() * db->get_per_hour_pay(name)) * db->get_pay_mod(name);
    double balance_now = db->get_balance(name);

    
    db->set_balance(name, balance_now + money_total);
    db->set_last_pay(name);
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(HttpStatusCode::k200OK);
    callback(resp);
}

