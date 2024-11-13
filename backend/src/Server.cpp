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

void api::respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(code);
    callback(resp);
}

void api::respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback, Json::Value js) {
    auto resp = HttpResponse::newHttpJsonResponse(js);
    resp->setStatusCode(code);
    callback(resp);
}

bool api::check_auth(const HttpRequestPtr &request, const std::string &name) {
    auto resp = HttpResponse::newHttpResponse();
    std::string tkn = request->getHeader("Authorization");

    if (!SessionManager::check_token_and_name(tkn, name)) {
        return false;
    }
    return true;
}
void api::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    std::cout << req->body() << std::endl;
    auto json_obj = req->getJsonObject();
    
    if (!json_obj) {
        respond(HttpStatusCode::k400BadRequest, std::move(callback));
        return;
    }
   
    const std::string name = (*json_obj)["name"].asString();
    const std::string password = (*json_obj)["password"].asString();

    if (name.empty() || password.empty()) { // If data is invalid
        respond(HttpStatusCode::k400BadRequest, std::move(callback));
        return;
    }


    Json::Value js;
    
    auto passwd = db->get_password(name);
    if (passwd) { // User does exist
        if (bcrypt::validatePassword(password, passwd.value())) {
            js["token"] = SessionManager::make_token(name);
        } else {
            respond(drogon::k401Unauthorized, std::move(callback));
        }
    } else { 
        respond(k400BadRequest, std::move(callback));
    }

    respond(k200OK, std::move(callback), js);
}
void api::login_token(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &token) {
    Json::Value js;
    if (token.empty()) {
        respond(HttpStatusCode::k401Unauthorized, std::move(callback));
        return;
    }

    if (!SessionManager::check_token(token)) {
        respond(k401Unauthorized, std::move(callback));
        return;
    }

    auto name_from_token = SessionManager::get_name_from_token(token);
    auto user_id = db->get_userid(name_from_token);

    if (!user_id) { // User does not exist, reset token on client
        respond(k401Unauthorized, std::move(callback));
        return;
    }
    
    js["name"] = name_from_token; 
    respond(k200OK, std::move(callback), js);
}

void api::reg(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (json_obj->empty()) {
        respond(drogon::k400BadRequest, std::move(callback));
        return;
    }

    const std::string name = (*json_obj)["name"].asString();
    const std::string password = (*json_obj)["password"].asString();

    if (name.empty() || password.empty() || name.size() > 16 || password.size() > 32) { // If data is not correct
        respond(k400BadRequest, std::move(callback));
        return;
    }

    if (!db->add_user(name, bcrypt::generateHash(password))) { // Create a new user
        respond(k400BadRequest, std::move(callback));
        return;
    } 

    Json::Value js;
    HttpStatusCode code = drogon::k200OK;
    
    js["token"] = SessionManager::make_token(name);
    respond(k200OK, std::move(callback), js);
}

void api::user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &name) {
    if (name.empty()) {
        respond(k400BadRequest, std::move(callback));
        return;
    }
    auto mods = db->get_modifiers(name);

    if (!mods) {
        std::cerr << "Error:can't get user info\n";
        respond(k400BadRequest, std::move(callback));
        return;
    }

    Json::Value js;
    js["balance"] = db->get_balance(name);
    js["click_mod"] = mods->click_modifier;
    js["hourly_payout"] = mods->hourly_payment;
    js["hourly_payout_mod"] = mods->hourly_payment_mod;
    js["last_pay"] = db->get_last_pay(name).value_or("NULL");
    js["click_mod_price"] = mods->click_mod_price;
    js["hourly_payout_mod_price"] = mods->hourly_payment_price;

    respond(k200OK, std::move(callback), js);
}

void api::clicks(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->jsonObject();
    

    if (!json_obj) {
        respond(drogon::k400BadRequest, std::move(callback));
        return;
    }
    const std::string name = (*json_obj)["name"].asString();
    const int click = std::stoi((*json_obj)["click"].asString());
    
    if (name.empty() || click == 0) {
        respond(k400BadRequest, std::move(callback));
        return;
    }

    if (!check_auth(req, name)) {
        respond(drogon::k401Unauthorized, std::move(callback));
        return;
    }

    db->add_clicks(name, click);
   
    respond(drogon::k200OK, std::move(callback));
}

void api::purchase(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (!json_obj) {
        respond(drogon::k400BadRequest, std::move(callback));
    }

    const std::string name = (*json_obj)["name"].asString();
    const std::string mod = (*json_obj)["modifier_type"].asString();

    if (name.empty() || mod.empty()) {
        respond(k400BadRequest, std::move(callback));
        return;
    }
    
   
    if (!check_auth(req, name)) {
        respond(drogon::k401Unauthorized, std::move(callback));
        return;
    }

    auto resp = HttpResponse::newHttpResponse();
    double bal = db->get_balance(name);

    auto mods = db->get_modifiers(name);
    if (!mods) {
        respond(k400BadRequest, std::move(callback));
        return;
    }


    if (mod == "click") {
        double cur_mod_price = mods->click_mod_price;
        if (bal < cur_mod_price) { // Not enough money
            respond(k400BadRequest, std::move(callback));
            return;
        }

        double cur_mod = mods->click_modifier;
        
        db->set_click_mod(name, cur_mod * 1.1);
        db->set_click_mod_price(name, cur_mod_price * 1.2);

        db->set_balance(name, bal - cur_mod_price);

        resp->setStatusCode(HttpStatusCode::k200OK);
    } else if (mod == "pay") {
        double cur_mod_price = mods->hourly_payment_price;
        if (bal < cur_mod_price) { // Not enough money
            respond(k400BadRequest, std::move(callback));
            return;
        }

        double cur_mod = mods->hourly_payment_mod;

        db->set_pay_mod(name, cur_mod * 1.1);
        db->set_pay_mod_price(name, cur_mod_price * 1.2);
        db->set_balance(name, bal - cur_mod_price);
        resp->setStatusCode(HttpStatusCode::k200OK);
    } 
    callback(resp);
}

void api::daily_pay(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (!json_obj) {
        respond(drogon::k400BadRequest, std::move(callback));
    }

    const std::string name = (*json_obj)["name"].asString();

    if (name.empty()) {
        respond(k400BadRequest, std::move(callback));
        return;
    }

    
    if (!check_auth(req, name)) {
        respond(k401Unauthorized, std::move(callback));
        return;
    }

    auto prev_time = db->get_last_pay(name);
    if (!prev_time) {
        db->set_last_pay(name);
        respond(k400BadRequest, std::move(callback));
        return;
    }
    
    auto mods = db->get_modifiers(name);
    if (!mods) {
        respond(drogon::k200OK, std::move(callback));
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

    double money_total = (hrs_passed.count() * mods->hourly_payment) * mods->hourly_payment_mod;
    double balance_now = db->get_balance(name);

    
    db->set_balance(name, balance_now + money_total);
    db->set_last_pay(name);
  
    respond(k200OK, std::move(callback));
}

