#include "EventLoop.h"
#include "TcpServer.h"
#include "deamon.h"
#include "log/log.h"


int main(){
    init("WebServer"); // init log modules
    //LOG_DEBUG << "init start";
    //create_deamon();
    EventLoop loop;
    //loop.loop();
    TcpServer server(&loop, 3, 80);
    server.start();
    loop.loop();
    return 0;
} 
