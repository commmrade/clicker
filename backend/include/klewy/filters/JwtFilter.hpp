#include<drogon/HttpFilter.h>
#include <klewy/SessionManager.hpp>


using namespace drogon;

class JwtAuthFilter : public HttpFilter<JwtAuthFilter> {
    void doFilter(const HttpRequestPtr &req, FilterCallback &&fcb, FilterChainCallback &&fccb) override;
};
