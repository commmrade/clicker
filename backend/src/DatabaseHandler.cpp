#include <klewy/DatabaseHandler.hpp>

DatabaseHandler::DatabaseHandler(const std::string ip, const std::string username, const std::string passwd, const std::string scheme) {
    driver = get_driver_instance();
    con = driver->connect(ip, username, passwd);
    con->setSchema(scheme);
}
DatabaseHandler::~DatabaseHandler() {
    delete con;
}

std::string DatabaseHandler::get_password(const std::string &name) {
    con->setSchema("clicker");
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();

    std::string query = "SELECT password FROM users WHERE username='" + name + "'";
    res = stmt->executeQuery(query);
    while (res->next()) {
        std::string result = res->getString(1);
        delete stmt;
        delete res;
        return result;
    }
    delete stmt;
    delete res;
    return "";
}
double DatabaseHandler::get_balance(const std::string &name) {
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    std::string query = "SELECT balance FROM game WHERE name='" + name + "'";
    res = stmt->executeQuery(query);

    while (res->next()) {
        double bal = res->getDouble(1);
        delete stmt;
        delete res;

        return bal;
    }
    delete stmt;
    delete res;
    return -1.0;
}
double DatabaseHandler::get_click_modifier(const std::string &name) {
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    std::string query = "SELECT click_modifier FROM game WHERE name='" + name + "'";
    res = stmt->executeQuery(query);

    while (res->next()) {
        double modifier = res->getDouble(1);
        delete stmt;
        delete res;

        return modifier;
    }
    delete stmt;
    delete res;
    return -1.0;
}
double DatabaseHandler::get_per_hour_pay(const std::string &name) {
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    std::string query = "SELECT per_hour_pay FROM game WHERE name='" + name + "'";
    res = stmt->executeQuery(query);

    while (res->next()) {
        double payout = res->getDouble(1);
        delete stmt;
        delete res;

        return payout;
    }
    delete stmt;
    delete res;
    return -1.0;
}
double DatabaseHandler::get_pay_mod(const std::string &name) {
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    std::string query = "SELECT pay_modifier FROM game WHERE name='" + name + "'";
    res = stmt->executeQuery(query);
    while (res->next()) {
        double modifier = res->getDouble(1);
        delete stmt;
        delete res;
        return modifier;
    }
    delete stmt;
    delete res;
    return -1.0;
}
double DatabaseHandler::get_pay_mod_price(const std::string &name) {
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    std::string query = "SELECT pay_mod_price FROM game WHERE name='" + name + "'";
    res = stmt->executeQuery(query);

    while (res->next()) {
        double pay = res->getDouble(1);
        delete stmt;
        delete res;

        return pay;
    }
    delete stmt;
    delete res;
    return -1.0;
}
double DatabaseHandler::get_click_mod_price(const std::string &name) {
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    std::string query = "SELECT click_mod_price FROM game WHERE name='" + name + "'";
    res = stmt->executeQuery(query);

    while (res->next()) {
        double price = res->getDouble(1);
        delete stmt;
        delete res;

        return price;
    }
    delete stmt;
    delete res;
    return -1.0;
}
std::string DatabaseHandler::get_last_pay(const std::string &name) {
    sql::Statement *stmt;
    sql::ResultSet *res;
    stmt = con->createStatement();
    std::string query = "SELECT last_pay FROM game WHERE name='" + name + "'";
    res = stmt->executeQuery(query);

    while (res->next()) {
        std::string last_pay = res->getString(1);
        delete stmt;
        delete res;

        return last_pay;
    }
    delete stmt;
    delete res;
    return "";
}

void DatabaseHandler::set_click_mod(const std::string &name, const double new_click_mod) {
    double cur_click_mod = new_click_mod;

    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("UPDATE game SET click_modifier=? WHERE name=?");
    pstmt->setDouble(1, std::round(cur_click_mod * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();

    delete pstmt;
}
void DatabaseHandler::set_pay_mod(const std::string &name, const double new_pay_mod) {
    double cur_pay_mod = new_pay_mod;

    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("UPDATE game SET pay_modifier=? WHERE name=?");
    pstmt->setDouble(1, std::round(cur_pay_mod * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();

    delete pstmt;
}
void DatabaseHandler::set_click_mod_price(const std::string &name, const double click_mod_price) {
    double cur_click_mod_price = click_mod_price;

    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("UPDATE game SET click_mod_price=? WHERE name=?");
    pstmt->setDouble(1, std::round(cur_click_mod_price * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();

    delete pstmt;
}
void DatabaseHandler::set_pay_mod_price(const std::string &name, const double new_pay_mod_price) {
    double cur_pay_mod_price = new_pay_mod_price;

    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("UPDATE game SET pay_mod_price=? WHERE name=?");
    pstmt->setDouble(1, std::round(cur_pay_mod_price * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();

    delete pstmt;
}
void DatabaseHandler::set_balance(const std::string &name, const double sum) {
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("UPDATE game SET balance=? WHERE name=?");
    pstmt->setDouble(1, std::round(sum * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();
    delete pstmt;
}
void DatabaseHandler::set_last_pay(const std::string &name) {
    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("UPDATE game SET last_pay=? WHERE name=?");
    
    pstmt->setString(2, name);
    auto t = time(nullptr);
    auto tm = *localtime(&t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << std::endl;
    pstmt->setDateTime(1, ss.str());
    pstmt->executeUpdate();

    delete pstmt;
}

bool DatabaseHandler::add_user(const std::string &name, const std::string &pass) {
    if (!get_password(name).empty()) {  
        return false;
    }

    sql::PreparedStatement *pstmt;
    pstmt = con->prepareStatement("INSERT INTO users (username, password) VALUES (?, ?)");
    pstmt->setString(1, name);
    pstmt->setString(2, pass);
    pstmt->executeUpdate();
    //Setup default values for game table
    
    pstmt = con->prepareStatement("INSERT INTO game (name) VALUES (?)");
    pstmt->setString(1, name);
    //pstmt->setDouble(2, 0.0);
    pstmt->executeUpdate();
    delete pstmt;

    return true;
}
void DatabaseHandler::add_clicks(const std::string &name, int clicks) {
    double balance = get_balance(name);

    sql::PreparedStatement *pstmt;
    std::cout << "clicks " << clicks << std::endl;
    pstmt = con->prepareStatement("UPDATE game SET balance=? WHERE name=?");
    pstmt->setDouble(1, balance + std::round((clicks * get_click_modifier(name)) * 100.0f) / 100.0f);
    pstmt->setString(2, name);
    pstmt->executeUpdate();

    delete pstmt;
}
