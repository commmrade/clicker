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
    con->setSchema("clicker");
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
   
    std::string query = "SELECT password FROM users WHERE username='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
    while (res->next()) {
        std::string result = res->getString(1);
        return result;
    }

    return std::nullopt;
}
double DatabaseHandler::get_balance(const std::string &name) {
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::string query = "SELECT balance FROM game WHERE name='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

    while (res->next()) {
        double bal = res->getDouble(1);
        return bal;
    }
    return -1.0;
}
double DatabaseHandler::get_click_modifier(const std::string &name) {
    std::unique_ptr<sql::Statement> stmt(con->createStatement());

    std::string query = "SELECT click_modifier FROM game WHERE name='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

    while (res->next()) {
        double modifier = res->getDouble(1);
        return modifier;
    }
    return -1.0;
}
double DatabaseHandler::get_per_hour_pay(const std::string &name) {
    
    
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::string query = "SELECT per_hour_pay FROM game WHERE name='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

    while (res->next()) {
        double payout = res->getDouble(1);
        return payout;
    }
    return -1.0;
}
double DatabaseHandler::get_pay_mod(const std::string &name) {
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::string query = "SELECT pay_modifier FROM game WHERE name='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
    while (res->next()) {
        double modifier = res->getDouble(1);
        return modifier;
    }
    return -1.0;
}
double DatabaseHandler::get_pay_mod_price(const std::string &name) {
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::string query = "SELECT pay_mod_price FROM game WHERE name='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

    while (res->next()) {
        double pay = res->getDouble(1);
        return pay;
    }
    return -1.0;
}
double DatabaseHandler::get_click_mod_price(const std::string &name) {
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::string query = "SELECT click_mod_price FROM game WHERE name='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

    while (res->next()) {
        double price = res->getDouble(1);
        return price;
    }
    return -1.0;
}
std::optional<std::string> DatabaseHandler::get_last_pay(const std::string &name) {
    std::unique_ptr<sql::Statement> stmt(con->createStatement());
    std::string query = "SELECT last_pay FROM game WHERE name='" + name + "'";
    std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));

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

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE game SET click_modifier=? WHERE name=?"));
    pstmt->setDouble(1, std::round(cur_click_mod * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();
}
void DatabaseHandler::set_pay_mod(const std::string &name, const double new_pay_mod) {
    double cur_pay_mod = new_pay_mod;

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE game SET pay_modifier=? WHERE name=?"));
    pstmt->setDouble(1, std::round(cur_pay_mod * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();
}
void DatabaseHandler::set_click_mod_price(const std::string &name, const double click_mod_price) {
    double cur_click_mod_price = click_mod_price;

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE game SET click_mod_price=? WHERE name=?"));
    pstmt->setDouble(1, std::round(cur_click_mod_price * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();
}
void DatabaseHandler::set_pay_mod_price(const std::string &name, const double new_pay_mod_price) {
    double cur_pay_mod_price = new_pay_mod_price;

    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE game SET pay_mod_price=? WHERE name=?"));
    pstmt->setDouble(1, std::round(cur_pay_mod_price * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();
}
void DatabaseHandler::set_balance(const std::string &name, const double sum) {
    std::cout << sum << std::endl;
    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE game SET balance=? WHERE name=?"));
    pstmt->setDouble(1, std::round(sum * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();
}
void DatabaseHandler::set_last_pay(const std::string &name) {
    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE game SET last_pay=? WHERE name=?"));
    
    pstmt->setString(2, name);
    auto t = time(nullptr);
    auto tm = *localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
    pstmt->setDateTime(1, ss.str());
    pstmt->executeUpdate();
}

bool DatabaseHandler::add_user(const std::string &name, const std::string &pass) {
    
    auto password_opt = get_password(name);

    if (password_opt) return false;

    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO users (username, password) VALUES (?, ?)"));
        pstmt->setString(1, name);
        pstmt->setString(2, pass);
        pstmt->executeUpdate();
    }
    
    {
        std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("INSERT INTO game (name, balance) VALUES (?, ?)"));
        pstmt->setString(1, name);
        pstmt->setDouble(2, 0.0);
        pstmt->executeUpdate();
    }
    return true;
}
void DatabaseHandler::add_clicks(const std::string &name, int clicks) {

    auto balance = get_balance(name);

    std::cout << "clicks " << clicks << std::endl;
    std::unique_ptr<sql::PreparedStatement> pstmt(con->prepareStatement("UPDATE game SET balance=? WHERE name=?"));
    pstmt->setDouble(1, balance + std::round((clicks * get_click_modifier(name)) * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();
     
}
