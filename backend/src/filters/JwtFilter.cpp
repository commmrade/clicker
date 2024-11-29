#include "klewy/filters/JwtFilter.hpp"
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <exception>


void JwtAuthFilter::doFilter(const HttpRequestPtr &req, FilterCallback &&fcb, FilterChainCallback &&fccb) {
    std::string auth_token = req->getHeader("Authorization");
    
    if (!auth_token.empty() && SessionManager::check_token(auth_token)) { // if success
        
        fccb();
        return;
    }

    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k401Unauthorized);
    try {
        fcb(resp);
    } catch (std::exception &ex) { // Catching bad_call so the app doesn't crash
        std::cout << ex.what() << std::endl;
    }
    
}
