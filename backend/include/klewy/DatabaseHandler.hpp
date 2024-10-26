#include "jdbc/cppconn/connection.h"
#include <cmath>
#include <iomanip>
#include "jdbc/cppconn/driver.h"
#include <optional>
#include <string>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <sstream>

class DatabaseHandler {
private:
    sql::Driver *driver;
    sql::Connection *con;
public:

    DatabaseHandler(const std::string ip, const std::string username, const std::string passwd, const std::string scheme);
    ~DatabaseHandler();

    std::optional<std::string> get_password(const std::string &name);
    double get_balance(const std::string &name);
    double get_click_modifier(const std::string &name);
    double get_per_hour_pay(const std::string &name);
    double get_pay_mod(const std::string &name);
    double get_pay_mod_price(const std::string &name);
    double get_click_mod_price(const std::string &name);
    std::optional<std::string> get_last_pay(const std::string &name);

    void set_click_mod(const std::string &name, const double new_click_mod);
    void set_pay_mod(const std::string &name, const double new_pay_mod);
    void set_click_mod_price(const std::string &name, const double click_mod_price);
    void set_pay_mod_price(const std::string &name, const double new_pay_mod_price);
    void set_balance(const std::string &name, const double sum);
    void set_last_pay(const std::string &name);
    
    bool add_user(const std::string &name, const std::string &pass);
    void add_clicks(const std::string &name, int clicks);
};