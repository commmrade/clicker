#include "jwt-cpp/traits/kazuho-picojson/defaults.h"
#include <chrono>
#include <exception>
#include<jwt-cpp/jwt.h>


class SessionManager {
public:
    static std::string make_token(const std::string &name);
    static bool check_token_and_name(const std::string &token, const std::string &name);
    static bool check_token(const std::string &key);
    static std::string get_name_from_token(const std::string &token);
private:

};