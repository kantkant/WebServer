#include "Channel.h"
#include <iostream>
#include <memory>

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
     fd_(fd),
     events_(0) {}

Channel::Channel(EventLoop* loop)
    :loop_(loop),
     fd_(0),
     events_(0) {}

Channel::~Channel() {}

void Channel::setReadcallback(CallBack&& cb) { readcallback_ = std::move(cb); }

void Channel::setWritecallback(CallBack&& cb) { writecallback_ = std::move(cb); }

void Channel::setConncallback(CallBack&& cb) { conncallback_ = std::move(cb); }

void Channel::setErrorcallback(CallBack&& cb) { errorcallback_ = std::move(cb); }

void Channel::setCloseCallBack(CallBack&& cb) { closecallback_ = std::move(cb); } //extra option, may not work

void Channel::handleRead() {
    if(readcallback_) {
        readcallback_();
    }
    updatePoller(this);
}

void Channel::handleWrite() {
    if(writecallback_) {
        writecallback_();
    }
}

void Channel::handleConn() { //abandon
    addtoPoller(this);
    std::cout << "handleconn1" <<  std::endl;
    if(conncallback_) {
        std::cout << "handleconn" << std::endl;
        conncallback_();
    }
}

void Channel::handleError() {
    if(errorcallback_) {
        errorcallback_();
    }
}

void Channel::handleClose() {
    if(closecallback_) {
        closecallback_();
    }
} //extra option, may not work

void Channel::setEvents(__uint32_t event) {
    events_ = event; 
    //setlastEvents(events_); //abandon
}

__uint32_t Channel::getEvents() { return events_; } //should be a reference type

//void Channel::setlastEvents(__uint32_t event) { lastevents_ = event; } 

//__uint32_t Channel::getlastEvents() { return lastevents_; }

void Channel::setHolder(std::shared_ptr<HttpConn> httpconn) { holder_ = httpconn; }

std::shared_ptr<HttpConn> Channel::getHolder() {
    std::shared_ptr<HttpConn> tmp(holder_.lock());
    return tmp;
}

void Channel::setFd(int fd) { fd_ = fd; }

int Channel::getFd() { return fd_; }

void Channel::handleEvents() {
    if((events_ & EPOLLHUP) && !(events_ & EPOLLIN)) {
        handleClose();
    }
    else if(events_ & EPOLLERR) {
        handleError();
    }
    else if(events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        handleRead();
    }
    else if(events_ & EPOLLOUT) {
        handleWrite();
    }
}

void Channel::updatePoller(Channel* channel) {
    std::shared_ptr<Channel> ch = loop_->epoller_->getChannel(fd_);
    loop_->epoller_->epoll_mod(ch);
}

void Channel::addtoPoller(Channel* channel) {
    std::shared_ptr<Channel> ch = loop_->epoller_->getChannel(fd_);
    loop_->epoller_->epoll_add(ch);
}