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
    void handleEvents(TimerManager timerManager);
    void handleClose();
    void handleTimer(TimerManager timerManager);
    void addTimer();
    void linkTimer(std::shared_ptr<TimerNode> timernode);
    void untieTimer();
    void setExpTime(int timeout);
    int getExpTime() const;
public:
    void setEvents(__uint32_t events);
    __uint32_t getEvents();
public:
    void setHolder(std::shared_ptr<HttpConn> httpconn);
    std::shared_ptr<HttpConn> getHolder();
public:
    void setFd(int fd);
    int getFd();
    
private:
    __uint32_t events_;
    EventLoop* loop_;
    int fd_;
    std::weak_ptr<HttpConn> holder_;
    CallBack readcallback_;
    CallBack writecallback_;
    CallBack errorcallback_;
    CallBack conncallback_; //work?
    CallBack closecallback_;
    int expiredTime_;
    std::weak_ptr<TimerNode> timer_;
};