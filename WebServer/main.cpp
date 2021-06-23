#include <iostream>
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "./base/Util.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "Channel.h"
#include "TcpServer.h"
#include "HttpConn.h"
#include <sys/eventfd.h>


int main(){  
    EventLoop loop;
    //loop.loop();
    TcpServer server(&loop, 3, 80);
    server.start();
    loop.loop();
    return 0;
} 
