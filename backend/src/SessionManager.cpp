#include "jwt-cpp/jwt.h"
#include <chrono>
#include <exception>
#include <klewy/SessionManager.hpp>


std::string SessionManager::make_key(const std::string &name) {
    std::string secret_key {"niggas"};

    auto token = jwt::create()
    .set_issued_now()
    .set_expires_in(std::chrono::seconds(36000))
    .set_payload_claim("name", jwt::claim(name))
    .sign(jwt::algorithm::hs256{secret_key});

    return token;
}
bool SessionManager::check_key_and_name(const std::string &key, const std::string &name) {
    std::string secret_key {"niggas"};
    try {
        auto decoded = jwt::decode(key);
        if (decoded.get_payload_claim("name").as_string() != name) {
            return false;
        }

        auto verifier = jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{secret_key});
        

        verifier.verify(decoded);

        //std::cout << "TOKEN IS VALID\n";
        return true;

    } catch (std::exception &e) {
        std::cout << "verificatiln failed " << e.what() << std::endl;
    }

    return false;
} 
bool SessionManager::check_key(const std::string &key) {
    std::string secret_key {"niggas"};

    try {
        auto decoded = jwt::decode(key);
        if (decoded.get_payload_claim("name").as_string().empty()) {
            return false;
        }

        auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256{secret_key});

        verifier.verify(decoded);

        return true;
    } catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
    }
    return false;
}
std::string SessionManager::get_name(const std::string &key) {
    std::string result;

    try {
        auto decoded = jwt::decode(key);
        result = decoded.get_payload_claim("name").as_string();
    } catch (...) {
        std::cout << "exc\n";
    }
    return result;
}