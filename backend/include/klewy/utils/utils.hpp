#include<drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>


using namespace drogon;

namespace Utility {
    void respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback);
    void respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback, Json::Value js);
}
