#include "EventLoopThreadPool.h"
#include <iostream>

EventLoopThreadPool::EventLoopThreadPool(const int ThreadNum_, EventLoop* loop_)
    :numThreads_(ThreadNum_),
     baseloop_(loop_),
     next_(0) {
        if(numThreads_ < 0) {
            abort();
        }
    }

void EventLoopThreadPool::start() {
    for(int i = 0; i < numThreads_; ++i) {
        std::unique_ptr<EventLoopThread> tmpThread_(new EventLoopThread);
        EventLoop* tmpLoop_ = tmpThread_->startLoop();
        threads_.push_back(std::move(tmpThread_));
        loops_.push_back(tmpLoop_);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop() {
    if(next_ >= numThreads_) {
        next_ = 1;
        return loops_[0];
    }
    return loops_[next_++];

}

EventLoopThreadPool::~EventLoopThreadPool() {}