#include "Channel.h"
#include <iostream>
#include <memory>

Channel::Channel(EventLoop* loop, int fd)
    :loop_(loop),
     fd_(fd),
     events_(0),
     expiredTime_(5*60*1000) {
         //std::cout << "Channel construct" << std::endl;
    }

Channel::Channel(EventLoop* loop)
    :loop_(loop),
     fd_(0),
     events_(0) {}

Channel::~Channel() {
    close(fd_);
    //std::cout << "Channel distruct" << std::endl;
}

void Channel::setReadcallback(callBack&& cb) { readcallback_ = std::move(cb); }

void Channel::setWritecallback(callBack&& cb) { writecallback_ = std::move(cb); }

void Channel::setConncallback(callBack&& cb) { conncallback_ = std::move(cb); }

void Channel::setErrorcallback(callBack&& cb) { errorcallback_ = std::move(cb); }

void Channel::setCloseCallBack(callBack&& cb) { closecallback_ = std::move(cb); } //extra option, may not work

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

__uint32_t Channel::getEvents() const{ return events_; } //should be a reference type

void Channel::setHolder(std::shared_ptr<HttpConn> httpconn) { holder_ = httpconn; }

std::shared_ptr<HttpConn> Channel::getHolder() {
    std::shared_ptr<HttpConn> tmp(holder_.lock());
    return tmp;
}

std::shared_ptr<TimerNode> Channel::getTimer() {
    std::shared_ptr<TimerNode> tmp(timer_.lock());
    return tmp;
}

void Channel::setFd(int fd) { fd_ = fd; }

int Channel::getFd() const{ return fd_; }

void Channel::handleEvents(TimerNodeList &timerManager) {
    if(timer_.lock()) {
        //std::cout << "handleTimer" << std::endl;
        handleTimer(timerManager);
    }
    if(events_ & EPOLLHUP && !(events_ & EPOLLIN) && closecallback_) {  //&'s priority is higher then &&
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

void Channel::handleTimer(TimerNodeList &timerManager) {
    //untieTimer(); //priority queue can't support "find"
    std::shared_ptr<HttpConn> httpconn = getHolder();
    if(httpconn) {
        timerManager.modTimer(timer_.lock(), expiredTime_);
    }
    else {
        //httpconn distruct
    }
}

void Channel::linkTimer(std::shared_ptr<TimerNode> timernode) {
    //std::cout << timernode.use_count() << std::endl;
    timer_ = timernode;
    //std::cout << timer_.lock().use_count() << std::endl;
}
/*
void Channel::untieTimer() {
    if(timer_.lock()) {
        //std::cout << "untie" << std::endl;
        std::shared_ptr<TimerNode> my_timer(timer_.lock());
        my_timer->clearReq();
        timer_.reset();
        //std::cout << timer_.lock() << std::endl;
    }
}
*/
void Channel::setExpTime(int timeout) {
    expiredTime_ = timeout;
    if(timer_.lock()) {
        timer_.lock()->update(timeout);
    }
}

int Channel::getExpTime() const{
    return expiredTime_;
}