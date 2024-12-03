#include <klewy/ConnectionPool.hpp>


ConnectionPool::ConnectionPool(const std::string &ip, const std::string &username, const std::string &password, const std::string &scheme, int pool_size) {
    driver = get_driver_instance();
    for (auto i : std::views::iota(0, pool_size)) {
        auto con = driver->connect(ip, username, password);

        std::shared_ptr<sql::Connection> conn{con};

        conn->setSchema(scheme);
        connections.push(std::move(conn));
    }

}


ConnectionPool::~ConnectionPool() {
     
}


auto ConnectionPool::get_connection() -> std::optional<std::shared_ptr<sql::Connection>> {
    std::unique_lock lock{mtx};
    
    cond.wait(lock, [this] { return !connections.empty() && active_connections < connections.size(); });
    
    auto connection = connections.front();
    connections.pop();
    active_connections++;
    return connection;
}

void ConnectionPool::return_connection(std::shared_ptr<sql::Connection> &&connection) {
    std::lock_guard lock{mtx};

    if (active_connections > 0) {
        active_connections--;
        connections.push(std::move(connection));
    }
    cond.notify_all();
}
