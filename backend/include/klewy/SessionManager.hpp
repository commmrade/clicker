#include "jwt-cpp/traits/kazuho-picojson/defaults.h"
#include <chrono>
#include <exception>
#include<jwt-cpp/jwt.h>


class SessionManager {
public:
    static std::string make_key(const std::string &name);
    static bool check_key(const std::string &key, const std::string &name);
private:

};