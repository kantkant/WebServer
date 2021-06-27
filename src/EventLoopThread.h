#pragma once
#include "EventLoop.h"
#include <iostream>
#include "./base/Thread.h"
#include "./base/MutexLock.h"
#include "./base/Condition.h"
#include "noncopyable.h"

class EventLoopThread : noncopyable {  //deleted exiting, have no idea about it
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void ThreadFunc();
    EventLoop* loop_;
    Thread thread_;
    MutexLock mutex_;
    Condition condition_;
};