#include "jdbc/cppconn/connection.h"
#include <cmath>
#include "jdbc/cppconn/driver.h"
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <jdbc/cppconn/driver.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include<ranges>
#include <memory>
#include <optional>
#include<condition_variable>




class ConnectionPool {
private:
    sql::Driver *driver;
    std::queue<std::shared_ptr<sql::Connection>> connections;
    int active_connections{0};

    std::condition_variable cond;
    std::mutex mtx;
public:
    ConnectionPool(const std::string &ip, const std::string &username, const std::string &password, const std::string &scheme, int pool_size);

    ~ConnectionPool();

    auto get_connection() -> std::optional<std::shared_ptr<sql::Connection>>;
    void return_connection(std::shared_ptr<sql::Connection> &&connection);
};