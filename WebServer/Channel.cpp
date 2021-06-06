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
    //updatePoller(this);
}

void Channel::handleWrite() {
    if(writecallback_) {
        writecallback_();
    }
}
/*
void Channel::handleConn() { //abandon
    loop_->addtoPoller(shared_from_this());
    if(conncallback_) {
        conncallback_();
    }
}
*/

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
    if((events_ & EPOLLHUP) && !(events_ & EPOLLIN) && closecallback_) {  //& priority is higher then &&
        handleClose(); 
    }
    else if(events_ & EPOLLERR && errorcallback_) {
        handleError();
    }
    else if(events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP) && readcallback_) {
        handleRead();
    }
    else if(events_ & EPOLLOUT && writecallback_) {
        handleWrite();
    }
}
/*
void Channel::updatePoller(std::shared_ptr<Channel> channel) {
    //loop_->epoller_->epoll_mod(channel);
    loop_->updatePoller(channel);
}

void Channel::addtoPoller(std::shared_ptr<Channel> channel) {
    //loop_->epoller_->epoll_add(channel);
    loop_->addtoPoller(channel);
}

void Channel::removeFromPoller(std::shared_ptr<Channel> channel) {
    //loop_->epoller_->epoll_del(channel);
    loop_->removeFromPoller(channel);
}
*/