#include "jwt-cpp/jwt.h"
#include <chrono>
#include <exception>
#include <klewy/SessionManager.hpp>
#include <stdexcept>


std::string SessionManager::make_token(const std::string &name) {
    std::string secret_key {"vladivostok2000"};

    auto token = jwt::create()
    .set_issued_now()
    .set_expires_in(std::chrono::seconds(36000))
    .set_payload_claim("name", jwt::claim(name))
    .sign(jwt::algorithm::hs256{secret_key});

    return token;
}

bool SessionManager::check_token(const std::string &key) {
    std::string secret_key {"vladivostok2000"};

    try {
        auto decoded = jwt::decode(key);


        auto verifier = jwt::verify().allow_algorithm(jwt::algorithm::hs256{secret_key});

        verifier.verify(decoded);

        return true;
    } catch (std::exception &exception) {
        std::cout << exception.what() << std::endl;
    }
    return false;
}
std::string SessionManager::get_name_from_token(const std::string &key) {
    std::string result;

    try {
        auto decoded = jwt::decode(key);
        result = decoded.get_payload_claim("name").as_string();
    } catch (...) {
        throw std::runtime_error("Invald token");
    }
    return result;
}