#pragma once
#include <iostream>
#include "Epoll.h"
#include "EventLoop.h"
#include <memory>
#include <string>
#include <functional>
#include <sys/epoll.h>
#include "noncopyable.h"

class EventLoop;
class HttpConn;

class Channel : noncopyable ,public std::enable_shared_from_this<Channel> { //C++11
private:
    typedef std::function<void()> CallBack; //fix bug.should ahead of class
/*
public:
    void updatePoller(std::shared_ptr<Channel> channel);
    void addtoPoller(std::shared_ptr<Channel> channel);
    void removeFromPoller(std::shared_ptr<Channel> channel);
*/
public:
    Channel(EventLoop* loop, int fd);
    Channel(EventLoop* loop);
    ~Channel();
public:
    void setReadcallback(CallBack&& cb);
    void setWritecallback(CallBack&& cb);
    void setErrorcallback(CallBack&& cb);
    void setConncallback(CallBack&& cb);
    void setCloseCallBack(CallBack&& cb); //attention on this one
public:
    void handleRead();
    void handleWrite();
    void handleError();
    void handleConn();
    void handleEvents();
    void handleClose();
public:
    void setEvents(__uint32_t events);
    __uint32_t getEvents();
    //void setlastEvents(__uint32_t event); //abandon, it's not a reference
    //__uint32_t getlastEvents();
public:
    void setHolder(std::shared_ptr<HttpConn> httpconn);
    std::shared_ptr<HttpConn> getHolder();
public:
    void setFd(int fd);
    int getFd();
    
private:
    __uint32_t events_;
    //__uint32_t lastevents_;
    EventLoop* loop_;
    int fd_;
    std::weak_ptr<HttpConn> holder_;
    CallBack readcallback_;
    CallBack writecallback_;
    CallBack errorcallback_;
    CallBack conncallback_; //work?
    CallBack closecallback_;
};