#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <functional>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "klewy/Server.hpp"
#include <drogon/drogon.h>
using namespace drogon;
//Known bugs
// 1) If delete game table, it wont create it again, unless u truncate users

int main()
{

    LOG_INFO << "\nserv running\n";
    app().addListener("127.0.0.1", 8848).run();
}