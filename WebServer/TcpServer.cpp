#include "TcpServer.h"
#include <iostream>
#include "Util.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "EventLoopThreadPool.h"
#include "HttpConn.h"

TcpServer::TcpServer(EventLoop* loop, int threadnum, int port) 
    :loop_(loop), 
     threadNum_(threadnum), 
     port_(port),
     eventloopThreadpool_(new EventLoopThreadPool(threadNum_, loop_)),
     start_(false),
     listenFd_(socket_bind_listen(port_)) {
        setSocketNonBlocking(listenFd_);
        setSocketNodelay(listenFd_);
        //acceptChannel_(std::move(new Channel(loop_, listenFd_))); //wrong,
        acceptChannel_ = std::make_shared<Channel>(loop_, listenFd_); //behind setsocketnonblocking
        //std::cout << "listeningFd: " << listenFd_ << std::endl;
    }

TcpServer::~TcpServer() {}

void TcpServer::start() {
    acceptChannel_->setEvents(EPOLLIN|EPOLLET);
    acceptChannel_->setReadcallback(std::bind(&TcpServer::handleConnection, this));
    eventloopThreadpool_->start();
    loop_->epoller_->epoll_add(acceptChannel_);
    //std::cout << "TcpServer start" << std::endl;
}

void TcpServer::handleConnection() { //send work to channel
    //std::cout << "TcpServer new Connection" << std::endl;
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(struct sockaddr_in));
    socklen_t clientAddrLen = sizeof(clientAddr);
    int acceptFd = 0;
    while((acceptFd = accept(listenFd_, (struct sockaddr*)&clientAddr, &clientAddrLen)) > 0) {
        EventLoop* subLoop = eventloopThreadpool_->getNextLoop();
        //is necessary to limit max conn?
        if(setSocketNonBlocking(acceptFd) < 0) {
            //deal bad syscall
        }
        setSocketNodelay(acceptFd);
        std::shared_ptr<HttpConn> httpconn(new HttpConn(subLoop, acceptFd));
        subLoop->runInLoop(std::bind(&Epoll::setHttpConn, subLoop->epoller_, httpconn, acceptFd)); //avoid HttpConn distruct
        //subLoop->epoller_->setHttpConn(httpconn, acceptFd);
        subLoop->runInLoop(std::bind(&HttpConn::handleNewEvents, httpconn)); //set conn task in channel
    }
    acceptChannel_->setEvents(EPOLLIN|EPOLLET); //for subChannel to do : resgister epoller . done
}//think about callbacks in channel
