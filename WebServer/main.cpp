#include "EventLoop.h"
#include "TcpServer.h"
#include "deamon.h"


int main(){
    create_deamon();
    EventLoop loop;
    //loop.loop();
    TcpServer server(&loop, 3, 80);
    server.start();
    loop.loop();
    return 0;
} 
