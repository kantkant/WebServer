#pragma once
#include <iostream>
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "Util.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include "Channel.h"
#include "TcpServer.h"

void hello(){
    std::cout << "Hello, ssp offer!" << std::endl;
}

int main(){
    EventLoop loop;
    //loop.loop();
    TcpServer server(&loop, 5, 1234);
    server.start();
    loop.loop();
    sleep(100);
    return 0;
}