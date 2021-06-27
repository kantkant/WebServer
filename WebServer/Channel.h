#pragma once
#include <iostream>
#include "Epoll.h"
#include "EventLoop.h"
#include <memory>
#include <string>
#include <functional>
#include <sys/epoll.h>
#include "noncopyable.h"
#include "TimerNodeList.h"

class EventLoop;
class HttpConn;

class Channel : noncopyable ,public std::enable_shared_from_this<Channel> { //C++11
private:
    typedef std::function<void()> callBack; //fix bug.should ahead of class
public:
    Channel(EventLoop* loop, int fd);
    Channel(EventLoop* loop);
    ~Channel();
public:
    void setReadcallback(callBack&& cb);
    void setWritecallback(callBack&& cb);
    void setErrorcallback(callBack&& cb);
    void setConncallback(callBack&& cb);
    void setCloseCallBack(callBack&& cb); //attention on this one
public:
    void handleRead();
    void handleWrite();
    void handleError();
    void handleConn();
    void handleEvents(TimerNodeList &timerManager);
    void handleClose();
private:
    void handleTimer(TimerNodeList &timerManager);
public:
    void addTimer();
    void linkTimer(std::shared_ptr<TimerNode> timernode);
    //void untieTimer();
    void setExpTime(int timeout);
    int getExpTime() const;
public:
    void setEvents(__uint32_t events);
    __uint32_t getEvents() const;
public:
    void setHolder(std::shared_ptr<HttpConn> httpconn);
    std::shared_ptr<HttpConn> getHolder();
    std::shared_ptr<TimerNode> getTimer();
public:
    void setFd(int fd);
    int getFd() const;
private:
    __uint32_t events_;
    EventLoop* loop_;
    int fd_;
    std::weak_ptr<HttpConn> holder_;
    callBack readcallback_;
    callBack writecallback_;
    callBack errorcallback_;
    callBack conncallback_; //work?
    callBack closecallback_;
    int expiredTime_;
    std::weak_ptr<TimerNode> timer_;
};