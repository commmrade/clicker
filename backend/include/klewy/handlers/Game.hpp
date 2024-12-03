#include <ctime>
#include <drogon/HttpController.h>
#include <drogon/HttpFilter.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <json/reader.h>
#include <json/value.h>
#include <klewy/utils/utils.hpp>
#include <json/value.h>
#include <memory>
#include <string>
#include<bcrypt/bcrypt.h>
#include <klewy/DatabaseHandler.hpp>
#include <klewy/SessionManager.hpp>
using namespace drogon;
class api : public HttpController<api> {
private:
    std::unique_ptr<DatabaseHandler> db;
    //DatabaseHandler db;
public:
    api();

    
    METHOD_LIST_BEGIN

    METHOD_ADD(api::user, "/user?name={username}", Get, "JwtAuthFilter");
    METHOD_ADD(api::clicks, "/clicks", Post, "JwtAuthFilter");
    METHOD_ADD(api::daily_pay, "/daily-pay", Post, "JwtAuthFilter");
    METHOD_ADD(api::purchase, "/purchase", Post, "JwtAuthFilter");
   
    METHOD_LIST_END

    
protected:
   

    void user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &name);

    void clicks(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void purchase(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void daily_pay(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    

    // bool check_auth(const HttpRequestPtr &request, const std::string &name);
    // bool check_auth(const HttpRequestPtr &request) ;
};



