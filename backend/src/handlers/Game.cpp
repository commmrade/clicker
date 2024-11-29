#include<klewy/handlers/Game.hpp>


#define DEBUG
#ifdef DEBUG
#define log(X) std::cout << X << std::endl
#endif

api::api() {
    db = std::make_unique<DatabaseHandler>("tcp://127.0.0.1:3306", "klewy", "root", "clicker");
}


void api::user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &name) {

    if (name.empty()) {
        Utility::respond(k400BadRequest, std::move(callback));
        return;
    }

    auto mods = db->get_modifiers(name);

    if (!mods) {
        std::cerr << "Error:can't get user info\n";
        Utility::respond(k400BadRequest, std::move(callback));
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

    Utility::respond(k200OK, std::move(callback), js);
}

void api::clicks(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->jsonObject();
    
    if (!json_obj) { // Incorrect json
        Utility::respond(drogon::k400BadRequest, std::move(callback));
        return;
    }

    const std::string name = (*json_obj)["name"].asString();
    const int click = std::stoi((*json_obj)["click"].asString());
    
    if (name.empty() || click == 0) { // Incoorect data in json
        Utility::respond(k400BadRequest, std::move(callback));
        return;
    }

    db->add_clicks(name, click);
    Utility::respond(drogon::k200OK, std::move(callback));
}

void api::purchase(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (!json_obj) {
        Utility::respond(drogon::k400BadRequest, std::move(callback));
    }

    const std::string name = (*json_obj)["name"].asString();
    const std::string mod = (*json_obj)["modifier_type"].asString();

    if (name.empty() || mod.empty()) { // Incorrect data in json
        Utility::respond(k400BadRequest, std::move(callback));
        return;
    }

  
    

    auto mods = db->get_modifiers(name);
    if (!mods) {
        Utility::respond(k400BadRequest, std::move(callback));
        return;
    }

    double bal = db->get_balance(name); // Current balance

    if (mod == "click") {
        double cur_mod_price = mods->click_mod_price;
        if (bal < cur_mod_price) { // Not enough money
            Utility::respond(k400BadRequest, std::move(callback));
            return;
        }

        double cur_mod = mods->click_modifier;
        
        db->set_click_mod(name, cur_mod * 1.1);
        db->set_click_mod_price(name, cur_mod_price * 1.2);
        db->set_balance(name, bal - cur_mod_price); // Updating balance and mods

        Utility::respond(k200OK, std::move(callback));
        return;
    } else if (mod == "pay") {

        double cur_mod_price = mods->hourly_payment_price;
        if (bal < cur_mod_price) { // Not enough money
            Utility::respond(k400BadRequest, std::move(callback));
            return;
        }

        double cur_mod = mods->hourly_payment_mod;

        db->set_pay_mod(name, cur_mod * 1.1);
        db->set_pay_mod_price(name, cur_mod_price * 1.2);
        db->set_balance(name, bal - cur_mod_price);  // Updating balance and mods

        Utility::respond(k200OK, std::move(callback));
        return;
    } 
    Utility::respond(k400BadRequest, std::move(callback)); // Means incorrect modifier
}

void api::daily_pay(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto json_obj = req->getJsonObject();

    if (!json_obj) {
        Utility::respond(drogon::k400BadRequest, std::move(callback));
    }

    const std::string name = (*json_obj)["name"].asString();

    if (name.empty()) {
        Utility::respond(k400BadRequest, std::move(callback));
        return;
    }

    auto prev_time = db->get_last_pay(name);
    if (!prev_time) { // User hasn't got any payments yet (return 200 because this is normal)
        db->set_last_pay(name);
        Utility::respond(drogon::k200OK, std::move(callback));
        return;
    }
    
    auto mods = db->get_modifiers(name);
    if (!mods) {
        Utility::respond(drogon::k200OK, std::move(callback));
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

    double money_total = mods->hourly_payment *mods->hourly_payment_mod * hrs_passed.count();
    double balance_now = db->get_balance(name);
    
    db->set_balance(name, balance_now + money_total);
    db->set_last_pay(name);
  
    Utility::respond(k200OK, std::move(callback));
}

