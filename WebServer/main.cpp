#include <iostream>
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "Util.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "Channel.h"
#include "TcpServer.h"
#include "HttpConn.h"
#include <sys/eventfd.h>

void hello(){
    std::cout << "Hello, ssp offer!" << std::endl;
}

int main(){  
    EventLoop loop;
    //loop.loop();
    TcpServer server(&loop, 1, 1234);
    server.start();
    loop.loop();
    return 0;
} 