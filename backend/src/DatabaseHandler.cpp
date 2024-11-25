#include "jdbc/cppconn/prepared_statement.h"
#include "jdbc/cppconn/resultset.h"
#include "jdbc/cppconn/statement.h"
#include <exception>
#include <klewy/DatabaseHandler.hpp>
#include <memory>
#include <optional>

DatabaseHandler::DatabaseHandler(const std::string ip, const std::string username, const std::string passwd, const std::string scheme) {
    driver = get_driver_instance();
    con = driver->connect(ip, username, passwd);
    con->setSchema(scheme);
}
DatabaseHandler::~DatabaseHandler() {
    delete con;
}

std::optional<std::string> DatabaseHandler::get_password(const std::string &name) {
    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("SELECT password FROM users WHERE name = ?"));
    stmt->setString(1, name);
   
    
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
    while (res->next()) {
        std::string result = res->getString(1);
        return result;
    }

    return std::nullopt;
}
std::optional<int> DatabaseHandler::get_userid(const std::string &name) {
    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("SELECT id FROM users WHERE name = ?"));
    stmt->setString(1, name);
    
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

    while (res->next()) {
        int result = res->getInt(1);
        return result;
    }  
    std::cerr << "Returning user_id nullopt\n";
    return std::nullopt;
}
double DatabaseHandler::get_balance(const std::string &name) {
    auto user_id = get_userid(name);
    if (!user_id) {
        return -1.0;
    }

    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("SELECT balance FROM bank WHERE user_id = ?"));
    stmt->setInt(1, user_id.value());
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

    while (res->next()) {
        double bal = res->getDouble(1);
        return bal;
    }
    return -1.0;
}
std::optional<Modifiers> DatabaseHandler::get_modifiers(const std::string &name) {
    Modifiers mods;

    auto user_id = get_userid(name);
    if (!user_id) {
        return std::nullopt;
    }

    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("SELECT click_modifier, click_mod_price, hourly_payment, hourly_payment_price, hourly_payment_mod FROM modifiers WHERE user_id = ?"));
    stmt->setInt(1, user_id.value());

    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());
    
    while (res->next()) {
        mods.click_modifier = res->getDouble(1);
        mods.click_mod_price = res->getDouble(2);
        mods.hourly_payment = res->getDouble(3);
        mods.hourly_payment_price = res->getDouble(4);
        mods.hourly_payment_mod = res->getDouble(5);

        return mods;
    }
    std::cerr << "Returning mod nullopt\n";
    return std::nullopt;
}

std::optional<std::string> DatabaseHandler::get_last_pay(const std::string &name) {
    auto user_id = get_userid(name);
    if (!user_id) {
        return std::nullopt;
    }

    std::unique_ptr<sql::PreparedStatement> stmt(con->prepareStatement("SELECT last_pay FROM payments WHERE user_id = ?"));
    stmt->setInt(1, user_id.value());
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

    while (res->next()) {
        std::string last_pay = res->getString(1);
        if (last_pay.empty()) {
            return std::nullopt;
        }
        return last_pay;
    }
    return std::nullopt;
}

void DatabaseHandler::set_click_mod(const std::string &name, const double new_click_mod) {
    double cur_click_mod = new_click_mod;

    auto user_id = get_userid(name);
    if (!user_id) {
        std::cerr << "Error: Can't add click_mod";
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE modifiers SET click_modifier=? WHERE user_id=?"));
    pstmt->setDouble(1, std::round(cur_click_mod * 100.0f) / 100.0f);
    pstmt->setInt(2, user_id.value());
    pstmt->executeUpdate();
}
void DatabaseHandler::set_pay_mod(const std::string &name, const double new_pay_mod) {
    double cur_pay_mod = new_pay_mod;

    auto user_id = get_userid(name);
    if (!user_id) {
        std::cerr << "Error: Can't set pay_mod";
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE modifiers SET hourly_payment_mod=? WHERE user_id=?"));
    pstmt->setDouble(1, std::round(cur_pay_mod * 100.0f) / 100.0f);
    pstmt->setInt(2, user_id.value());
    pstmt->executeUpdate();
}
void DatabaseHandler::set_click_mod_price(const std::string &name, const double click_mod_price) {
    double cur_click_mod_price = click_mod_price;

    auto user_id = get_userid(name);
    if (!user_id) {
        std::cerr << "Error: Can't set click_mod_price";
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE modifiers SET click_mod_price=? WHERE user_id=?"));
    pstmt->setDouble(1, std::round(cur_click_mod_price * 100.0f) / 100.0f);
    pstmt->setInt(2, user_id.value());
    pstmt->executeUpdate();
}
void DatabaseHandler::set_pay_mod_price(const std::string &name, const double new_pay_mod_price) {
    double cur_pay_mod_price = new_pay_mod_price;

    auto user_id = get_userid(name);
    if (!user_id) {
        std::cerr << "Error: Can't set pay_md_price";
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE modifiers SET hourly_payment_price=? WHERE user_id=?"));
    pstmt->setDouble(1, std::round(cur_pay_mod_price * 100.0f) / 100.0f);
    pstmt->setInt(2, user_id.value());
    pstmt->executeUpdate();
}
void DatabaseHandler::set_balance(const std::string &name, const double sum) {
    auto user_id = get_userid(name);
    if (!user_id) {
        std::cerr << "Error: Can't set balance";
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE bank SET balance=? WHERE user_id=?"));
    pstmt->setDouble(1, std::round(sum * 100.0f) / 100.0f);
    pstmt->setInt(2, user_id.value());
    pstmt->executeUpdate();
}
void DatabaseHandler::set_last_pay(const std::string &name) {
    auto user_id = get_userid(name);
    if (!user_id) {
        std::cerr << "Error: Can't set last_pay";
        return;
    }
    
    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE payments SET last_pay=? WHERE user_id=?"));
    pstmt->setInt(2, user_id.value());
    auto t = time(nullptr);
    auto tm = *localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
    pstmt->setDateTime(1, ss.str());
    pstmt->executeUpdate();
}

bool DatabaseHandler::add_user(const std::string &name, const std::string &pass) {
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO users (name, password) VALUES (?, ?)"));
        pstmt->setString(1, name);
        pstmt->setString(2, pass);
        pstmt->executeUpdate();
    }
    auto user_id = get_userid(name);
    if (!user_id)  return false;

    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO bank (user_id, balance) VALUES (?, ?)"));
        pstmt->setInt(1, user_id.value());
        pstmt->setDouble(2, 0.0);
        pstmt->executeUpdate();
    }
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO modifiers (user_id) VALUES (?)"));
        pstmt->setInt(1, user_id.value());
        pstmt->executeUpdate();
    }
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO payments (user_id) VALUES (?)"));
        pstmt->setInt(1, user_id.value());
        pstmt->executeUpdate();
    }
    return true;
}
void DatabaseHandler::add_clicks(const std::string &name, int clicks) {

    auto balance = get_balance(name);

    auto mods = get_modifiers(name);

    if (!mods) {
        std::cerr << "Error: Can't add clicks\n";
        return;
    }

    auto user_id = get_userid(name);
    if (!user_id) {
        std::cerr << "Error: Can't add clicks";
        return;
    }

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE bank SET balance=? WHERE user_id=?"));
    pstmt->setDouble(1, balance + std::round((clicks * mods->click_modifier) * 100.0f) / 100.0f);
    pstmt->setInt(2, user_id.value());
    pstmt->executeUpdate();
     
}
