#pragma once
#include <iostream>
#include "EventLoopThread.h"
#include "EventLoop.h"
#include <vector>
#include <memory>
#include "noncopyable.h"

class EventLoopThreadPool : noncopyable {
public:
    EventLoopThreadPool(const int ThreadNum_, EventLoop* loop_);
    ~EventLoopThreadPool();
    EventLoop* getNextLoop();
    void start();
private:
    const int numThreads_;
    EventLoop* baseloop_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};