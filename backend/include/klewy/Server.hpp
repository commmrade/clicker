#include <chrono>
#include <ctime>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/drogon.h>
#include<optional>
#include <functional>
#include <iomanip>
#include <json/value.h>
#include <memory>
#include <numbers>
#include <sstream>
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

    METHOD_ADD(api::login, "/login", Post);
    METHOD_ADD(api::reg, "/reg", Post);
    METHOD_ADD(api::user, "/user?name={username}", Get);
    METHOD_ADD(api::clicks, "/clicks", Post);
    METHOD_ADD(api::daily_pay, "/daily-pay", Post);
    METHOD_ADD(api::purchase, "/purchase", Post);
    METHOD_ADD(api::login_token, "/login?token={tkn}", Get);
    
   
    METHOD_LIST_END

    
protected:
    void login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void login_token(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &token);
    void reg(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void user(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string &name);

    void clicks(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void purchase(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void daily_pay(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

    void respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback);
    void respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback, Json::Value js);

    bool check_auth(const HttpRequestPtr &request, const std::string &name);
    bool check_auth(const HttpRequestPtr &request) ;
};