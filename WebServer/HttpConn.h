#pragma once
#include <iostream>
#include "EventLoop.h"
#include "Channel.h"
#include "Timer.h"
#include <unordered_map>
#include "HttpServer.h"

enum ConnectionState {
    H_CONNECTED = 0,
    H_DISCONNECTING,
    H_DISCONNECTED
};

class HttpConn : noncopyable, public std::enable_shared_from_this<HttpConn> {
public:
    HttpConn(EventLoop* loop, int fd);
    ~HttpConn();
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();
    void handleNewEvents();
    void enableWriting();
    void enableReading();
private:
    std::string inBuffer_;
    std::string outBuffer_;
    std::shared_ptr<Channel> channel_;
    EventLoop* loop_;
public:
    ConnectionState connectionState_;
private:
    std::shared_ptr<HttpServer> httpServer_;
};