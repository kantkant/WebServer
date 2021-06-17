#include "HttpConn.h"
#include "Util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>

HttpConn::HttpConn(EventLoop* loop, int fd)
    :loop_(loop),
     channel_(new Channel(loop_, fd)),
     connectionState_(H_CONNECTED),
     httpServer_(new HttpServer()) {
         channel_->setReadcallback(std::bind(&HttpConn::handleRead, this));
         channel_->setWritecallback(std::bind(&HttpConn::handleWrite, this));
         channel_->setCloseCallBack(std::bind(&HttpConn::handleClose, this));
         std::cout << "httpConn construct" << std::endl;
    }

HttpConn::~HttpConn() { std::cout << "httpConn distruct" << std::endl;}

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
        loop_->queueInLoop(std::bind(&HttpServer::messageCallback, httpServer_, inBuffer_, outBuffer_)); //set TIME_OUT for one conn
    }
    else if(n == 0) {
        handleClose();
    }
    else {
        handleError();
    }
}

void HttpConn::handleWrite() {
    int outBufSize = outBuffer_.size();
    ssize_t n = writen(channel_->getFd(), outBuffer_);
    if(n > 0 && n == outBufSize) {
        loop_->queueInLoop(std::bind(&HttpServer::writeCompleteCallback, httpServer_));
    }
    if(n > 0 && connectionState_ == H_DISCONNECTING) {
        shutdown(channel_->getFd(), SHUT_WR);
    }
    if(n <= 0) {
        //bad syscall
    }
}

void HttpConn::handleNewEvents() {
    connectionState_ = H_CONNECTED;
    channel_->setHolder(shared_from_this());
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->addtoPoller(channel_);
    loop_->queueInLoop(std::bind(&HttpServer::connectionCallback, httpServer_, shared_from_this()));
    channel_->addTimer();
    //loop_->queueInLoop(std::bind(&Channel::addTimer, channel_));
}

void HttpConn::handleClose() {
    connectionState_ = H_DISCONNECTED;
    std::shared_ptr<HttpConn> guard(shared_from_this());
    channel_->untieTimer();
    loop_->queueInLoop(std::bind(&EventLoop::removeFromPoller, loop_, channel_));
    loop_->queueInLoop(std::bind(&HttpServer::closeCallback, httpServer_));
}

void HttpConn::handleError() {
    //std::cout << channel-getFd() << std::endl;
}
