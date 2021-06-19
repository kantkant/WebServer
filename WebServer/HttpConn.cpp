#include "HttpConn.h"
#include "Util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>
#include "HttpServer.h"



HttpConn::HttpConn(EventLoop* loop, int fd)
    :loop_(loop),
     channel_(new Channel(loop_, fd)),
     connectionState_(H_CONNECTED),
     httpServer_(new HttpServer()) {
         channel_->setReadcallback(std::bind(&HttpConn::handleRead, this));
         channel_->setWritecallback(std::bind(&HttpConn::handleWrite, this));
         channel_->setCloseCallBack(std::bind(&HttpConn::handleClose, this));
         //std::cout << "httpConn construct" << std::endl;
    }

HttpConn::~HttpConn() {} //std::cout << "httpConn distruct" << std::endl;}


void HttpConn::enableWriting() {
    channel_->setEvents(EPOLLOUT | EPOLLET);
    loop_->updatePoller(channel_);
}

void HttpConn::enableReading() {
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->updatePoller(channel_);
}

void HttpConn::handleRead() {
    ssize_t n = readn(channel_->getFd(), inBuffer_); //let channel get fd;
    if(n > 0) {
        httpServer_->messageCallback(inBuffer_, outBuffer_); //set TIME_OUT for one conn
    }   //use ref() as reference
    else if(n == 0) {
        handleClose();
    }
    else {
        handleError();
    }
}

void HttpConn::handleWrite() { //care about buffer free
    int outBufSize = outBuffer_.size();
    ssize_t n = writen(channel_->getFd(), outBuffer_);
    //std::cout << "write" << std::endl;
    if(n > 0 && n == outBufSize && connectionState_ != H_DISCONNECTING) {
        httpServer_->writeCompleteCallback();  //watch out !!!!! 6/18
        return;
    }
    //if(n > 0 && n < outBufSize && connectionState_ == H_DISCONNECTING) {}
    if(n > 0 && n == outBufSize && connectionState_ == H_DISCONNECTING) {
        handleClose();
    }
    if(n <= 0) {
        //bad syscall
        handleError();
    }
}

void HttpConn::handleNewEvents() {
    connectionState_ = H_CONNECTED;
    channel_->setHolder(shared_from_this());
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->addtoPoller(channel_);
    httpServer_->connectionCallback(shared_from_this());
    loop_->addTimer(channel_, channel_->getExpTime());
}

void HttpConn::handleClose() {
    shutdown(channel_->getFd(), SHUT_WR);
    connectionState_ = H_DISCONNECTED;
    std::shared_ptr<HttpConn> guard(shared_from_this());
    channel_->untieTimer();
    loop_->queueInLoop(std::bind(&EventLoop::removeFromPoller, loop_, channel_));
    httpServer_->closeCallback();
    //shutdown(channel_->getFd(), SHUT_WR);
}

void HttpConn::handleError() { //return 400 request
    //std::cout << channel-getFd() << std::endl;
    connectionState_ = H_DISCONNECTING;
    //conn->server : server->conn;
    
    //send in outBuffer_
}

void HttpConn::setExpTime(int timeout) {
    channel_->setExpTime(timeout);
}
