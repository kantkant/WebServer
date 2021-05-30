#include "EventLoop.h"
#include <iostream>
#include <sys/syscall.h>
#include <thread>
#include <assert.h>

__thread EventLoop* loopInthisThread_ = nullptr;
EventLoop::EventLoop()
    :looping_(false),
     quit_(false),
     threadId_(std::this_thread::get_id()) { //C++11 with std::thread::get_id()
        //assert(this == nullptr);  can't use assert in constructor
        if(!loopInthisThread_) {loopInthisThread_ = this;}
    }

void EventLoop::loop() {
    assert(isloopInthisThread());
    while(!quit_) {
        std::cout << "Thread Id is " << threadId_ << "!" <<std::endl;
        sleep(2);
    }     
}

bool EventLoop::isloopInthisThread() const {return threadId_ == std::this_thread::get_id();}; //can't reverse

void EventLoop::quit() { quit_ = true;};

EventLoop::~EventLoop() {
    quit();
    loopInthisThread_ = nullptr;
}
