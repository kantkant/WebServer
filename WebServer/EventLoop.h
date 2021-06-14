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
    void addtoPoller(std::shared_ptr<Channel> channel, int timeout = 0);
    void updatePoller(std::shared_ptr<Channel> channel, int timeout = 0);
    void removeFromPoller(std::shared_ptr<Channel> channel);
private:
    bool isloopInthisThread() const;
    void handleRead();
    void wakeup();
    void queueInLoop(funcCallback&& cb);
    int createEventFd();
    int wakeupFd_;
    const std::thread::id threadId_;
    bool looping_;
    bool quit_;
    MutexLock mutex_;
    std::vector<funcCallback> pendingfunctors_;
    bool callingPendingFunctors_;
    std::shared_ptr<Channel> wakeupChannel_;
public:
    std::shared_ptr<Epoll> epoller_;
};