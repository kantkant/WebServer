#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>
#include <thread>
#include "Epoll.h"
#include "noncopyable.h"
#include "MutexLock.h"
#include "Condition.h"
#include <sys/eventfd.h>
#include <functional>

class Epoll; //fix bug
class Channel; //fix bug

class EventLoop : noncopyable {
public:
    typedef std::function<void()> funcCallback;
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void runInLoop(funcCallback&& cb);
    void doPendingFunctors();
private:
    bool isloopInthisThread() const;
    void handleRead();
    void queueInLoop(funcCallback&& cb);
    void wakeup();
    int createEventFd();
    void addtoPoller(Channel* channel);
    const std::thread::id threadId_;
    bool looping_;
    bool quit_;
    MutexLock mutex_;
    std::vector<funcCallback> pendingfunctors_;
    bool callingPendingFunctors_;
    std::shared_ptr<Channel> wakeupChannel_;
    int wakeupFd_;
public:
    std::unique_ptr<Epoll> epoller_;
};