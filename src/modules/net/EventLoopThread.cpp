#include <iostream>
#include "EventLoopThread.h"


EventLoopThread::EventLoopThread() 
    :loop_(nullptr),
     thread_(std::bind(&EventLoopThread::ThreadFunc, this)),
     mutex_(),
     condition_(mutex_) {}

void EventLoopThread::ThreadFunc() {
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        condition_.notify();
    }
    loop.loop();
}

EventLoop* EventLoopThread::startLoop() {
    thread_.start();
    MutexLockGuard lock(mutex_);
    while(loop_ == nullptr){
        condition_.wait();
    }
    return loop_;
}

EventLoopThread::~EventLoopThread() {
    loop_->quit();
    thread_.join();
    loop_ = nullptr;
}
