#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include "noncopyable.h"


class EventLoop : noncopyable{
public:
    EventLoop();
    ~EventLoop();
    void loop();
private:
    bool isloopInthisThread() const;
    void quit();  
    const std::thread::id threadId_;
    bool looping_;
    bool quit_;
};