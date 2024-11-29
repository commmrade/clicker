#include <drogon/HttpController.h>
#include <klewy/utils/utils.hpp>
#include <klewy/SessionManager.hpp>
#include <bcrypt/bcrypt.h>
#include <klewy/DatabaseHandler.hpp>
using namespace drogon;
class auth : public HttpController<auth> {
public:
    auth();
    

    METHOD_LIST_BEGIN

    METHOD_ADD(auth::login, "/login", Post);
    METHOD_ADD(auth::reg, "/reg", Post);
    METHOD_ADD(auth::login_token, "/login", Get, "JwtAuthFilter");
   
    METHOD_LIST_END

protected:
    void login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void login_token(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
    void reg(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);

private:
    std::unique_ptr<DatabaseHandler> db;
};