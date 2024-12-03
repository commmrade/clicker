#include <klewy/utils/utils.hpp>



void Utility::respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(code);
    callback(resp);
}

void Utility::respond(HttpStatusCode code, std::function<void(const HttpResponsePtr &)> &&callback, Json::Value js) {
    auto resp = HttpResponse::newHttpJsonResponse(js);
    resp->setStatusCode(code);
    callback(resp);
}