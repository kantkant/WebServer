#pragma once
#include <iostream>
#include "EventLoop.h"
#include "Channel.h"

class HttpConn : noncopyable, public std::enable_shared_from_this<HttpConn> {
public:
    HttpConn(EventLoop* loop, int fd);
    ~HttpConn();
    std::shared_ptr<Channel> getChannel();
    void handleRead();
    void handleWrite();
    void tie();
    void handleNewEvents();
    void handleClose();
private:
    int fd_;
    std::string inBuffer_;
    std::string outBuffer_;
    std::shared_ptr<Channel> channel_;
    EventLoop* loop_;
};