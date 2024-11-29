#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <functional>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <drogon/drogon.h>
using namespace drogon;


int main()
{
   
    LOG_INFO << "\nserv running\n";
    app().addListener("0.0.0.0", 8848).run();
}