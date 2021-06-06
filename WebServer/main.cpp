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
#include "HttpConn.h"

void hello(){
    std::cout << "Hello, ssp offer!" << std::endl;
}

int main(){
    EventLoop loop;
    //std::cout << "mainLoop:" << &loop <<std::endl;
    TcpServer server(&loop, 1, 1234);
    server.start();
    int fd = socket_bind_listen(1234); //not accept fd
    //std::shared_ptr<HttpConn> conn(new HttpConn(&loop, fd));
    //ch->setReadcallback(std::bind(hello));
    //conn->getChannel()->setEvents(EPOLLIN|EPOLLET);
    //loop.epoller_->epoll_add(conn->getChannel());
    loop.loop();
    //std::vector<int> a;
    //a.resize(100);
    //std:: cout <<a.size(); 
    sleep(100);
    return 0;
} 