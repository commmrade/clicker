#include<jwt-cpp/jwt.h>


class SessionManager {
public:
    static std::string make_token(const std::string &name);
    

    static bool check_token(const std::string &key);
    static std::string get_name_from_token(const std::string &token);
private:

};