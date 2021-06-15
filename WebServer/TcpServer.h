#pragma once
#include <iostream>
#include "noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "Channel.h"
#include "HttpConn.h"

class TcpServer : noncopyable {
public:
    TcpServer(EventLoop* loop, int threadnum, int port);
    ~TcpServer();

    void handleConnection();
    void updatePoller();
    void start();
    EventLoop* getLoop();

private:
    EventLoop* loop_;
    int threadNum_;
    std::unique_ptr<EventLoopThreadPool> eventloopThreadpool_;
    std::shared_ptr<Channel> acceptChannel_;
    bool start_;
    int port_;
    int listenFd_;
    int idleFd_;
};