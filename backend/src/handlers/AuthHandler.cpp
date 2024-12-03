#include <klewy/handlers/AuthHandler.hpp>


auth::auth() {
    db = std::make_unique<DatabaseHandler>("tcp://127.0.0.1:3306", "klewy", "root", "clicker");
}

void auth::login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {

    auto json_obj = req->getJsonObject();
    
    if (!json_obj) {
        Utility::respond(HttpStatusCode::k400BadRequest, std::move(callback));
        return;
    }
   
    const std::string name = (*json_obj)["name"].asString();
    const std::string password = (*json_obj)["password"].asString();

    if (name.empty() || password.empty()) { // If data is invalid
        Utility::respond(HttpStatusCode::k400BadRequest, std::move(callback));
        return;
    }
    
    auto passwd = db->get_password(name);
    if (!passwd || !bcrypt::validatePassword(password, passwd.value())) { // User doesn't exist or password is wrong
        Utility::respond(k401Unauthorized, std::move(callback));
        return;
    }

    Json::Value js;
    js["token"] = SessionManager::make_token(name);
    Utility::respond(k200OK, std::move(callback), js);
}
void auth::login_token(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    
    auto token = req->getHeader("Authorization");
    auto name_from_token = SessionManager::get_name_from_token(token);
    auto user_id = db->get_userid(name_from_token);
    
    if (!user_id) { // User does not exist, reset token on client
        Utility::respond(k401Unauthorized, std::move(callback));
        return;
    }

    Json::Value js;
    js["name"] = name_from_token; 
    Utility::respond(k200OK, std::move(callback), js);
}

void auth::reg(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (json_obj->empty()) {
        Utility::respond(drogon::k400BadRequest, std::move(callback));
        return;
    }

    const std::string name = (*json_obj)["name"].asString();
    const std::string password = (*json_obj)["password"].asString();

    if (name.empty() || password.empty() || name.size() > 16 || password.size() > 32) { // If data is not correct
        Utility::respond(k400BadRequest, std::move(callback));
        return;
    }

    if (!db->add_user(name, bcrypt::generateHash(password))) { // Create a new user if one doesn't exist
        Utility::respond(k400BadRequest, std::move(callback));
        return;
    } 

    Json::Value js;
    HttpStatusCode code = drogon::k200OK;
    js["token"] = SessionManager::make_token(name);
    Utility::respond(k200OK, std::move(callback), js);
}
