#include "Channel.h"
#include <iostream>
#include <memory>



Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
     fd_(fd),
     events_(0),
     expiredTime_(0) {}

Channel::Channel(EventLoop* loop)
    :loop_(loop),
     fd_(0),
     events_(0) {}

Channel::~Channel() {
    close(fd_);
}

void Channel::setReadcallback(CallBack&& cb) { readcallback_ = std::move(cb); }

void Channel::setWritecallback(CallBack&& cb) { writecallback_ = std::move(cb); }

void Channel::setConncallback(CallBack&& cb) { conncallback_ = std::move(cb); }

void Channel::setErrorcallback(CallBack&& cb) { errorcallback_ = std::move(cb); }

void Channel::setCloseCallBack(CallBack&& cb) { closecallback_ = std::move(cb); } //extra option, may not work

void Channel::handleRead() {
    if(readcallback_) {
        readcallback_();
    }
}

void Channel::handleWrite() {
    if(writecallback_) {
        writecallback_();
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
}

void Channel::setEvents(__uint32_t event) {
    events_ = event; 
}

__uint32_t Channel::getEvents() { return events_; } //should be a reference type

void Channel::setHolder(std::shared_ptr<HttpConn> httpconn) { holder_ = httpconn; }

std::shared_ptr<HttpConn> Channel::getHolder() {
    std::shared_ptr<HttpConn> tmp(holder_.lock());
    return tmp;
}

void Channel::setFd(int fd) { fd_ = fd; }

int Channel::getFd() { return fd_; }

void Channel::handleEvents(TimerManager timerManager) {
    if(holder_.lock()) {
        handleTimer(timerManager);
    }
    if(events_ & EPOLLHUP && !(events_ & EPOLLIN) && closecallback_) {  //&'s priority is higher then &&
        handleClose(); 
    }
    if(events_ & EPOLLERR && errorcallback_) {
        handleError();
    }
    if(events_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP) && readcallback_) {
        handleRead();
    }
    if(events_ & EPOLLOUT && writecallback_) {
        handleWrite();
    }
}

void Channel::handleTimer(TimerManager timerManager) {
    untieTimer(); //priority queue can't support "find"
    std::shared_ptr<HttpConn> httpconn = getHolder();
    if(httpconn) {
        timerManager.addTimer(shared_from_this(), expiredTime_);
    }
    else {
        //httpconn distruct
    }
}

void Channel::linkTimer(std::shared_ptr<TimerNode> timernode) {
    timer_ = timernode;
}

void Channel::untieTimer() {
    if(timer_.lock()) {
        std::shared_ptr<TimerNode> my_timer(timer_.lock());
        my_timer->clearReq();
        timer_.reset();
    }
}