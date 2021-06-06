#include "HttpConn.h"
#include "Util.h"
#include <memory>

HttpConn::HttpConn(EventLoop* loop, int fd)
    :loop_(loop),
     fd_(fd),
     channel_(new Channel(loop_, fd_)) {
         //channel_->setHolder(shared_from_this()); //c++11
         channel_->setReadcallback(std::bind(&HttpConn::handleRead, this));
         channel_->setWritecallback(std::bind(&HttpConn::handleWrite, this));
         channel_->setCloseCallBack(std::bind(&HttpConn::handleClose, this));
         std::cout << "construct" << std::endl;
    }

HttpConn::~HttpConn() { std::cout << "done" << std::endl; };

void HttpConn::tie() {
    channel_->setHolder(shared_from_this()); //can not set in constructor
}


std::shared_ptr<Channel> HttpConn::getChannel() {
    return channel_;
}

void HttpConn::handleRead() {
    int fd = getChannel()->getFd();
    inBuffer_.clear();
    int n = readn(fd, inBuffer_);
    if(n > 0) { //if (n > 0), change events to write; else change to close
        std::cout << inBuffer_ << std::endl;
        channel_->setEvents(EPOLLOUT | EPOLLET);
        loop_->updatePoller(channel_);
    }
    else {  //take some conditions;
        channel_->handleClose();
    }
}

void HttpConn::handleWrite() {
    int fd = getChannel()->getFd();
    outBuffer_.clear();
    outBuffer_ = inBuffer_ + ": Right handleWrite!";
    int n = writen(fd, outBuffer_);
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->updatePoller(channel_);
}

void HttpConn::handleNewEvents() {
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->addtoPoller(channel_);
    std::cout << "new Events handle OK" << std::endl;
}

void HttpConn::handleClose() {
    std::shared_ptr<HttpConn> guard(shared_from_this());
    loop_->removeFromPoller(channel_);
    close(fd_);
}
