#include "EventLoop.h"
#include <iostream>
#include <sys/syscall.h>
#include <thread>
#include <assert.h>
#include "./base/Util.h"
#include "Channel.h"
#include "./base/MutexLock.h"
#include "./base/Condition.h"
#include "log/log.h"


__thread EventLoop* loopInthisThread_ = nullptr;
EventLoop::EventLoop()
    :looping_(false),
     quit_(false),
     epoller_(new Epoll(this)), //fix bug
     threadId_(std::this_thread::get_id()),
     callingPendingFunctors_(false),
     wakeupFd_(createEventFd()), 
     wakeupChannel_(new Channel(this, wakeupFd_)) { //C++11 with std::thread::get_id()
        //assert(this == nullptr);  can't use in constructor, why?
        LOG_DEBUG << "event loop start";
        LOG_INFO << "loopInthisThread : " << loopInthisThread_
                 << "this : " << this;
        if(!loopInthisThread_) {loopInthisThread_ = this;}
        wakeupChannel_->setEvents(EPOLLIN | EPOLLET);
        wakeupChannel_->setReadcallback(std::bind(&EventLoop::handleRead, this)); //watch out std::bind
        addtoPoller(wakeupChannel_);
    }

void EventLoop::loop() {
    assert(isloopInthisThread());
    while(!quit_) {
        std::vector<std::shared_ptr<Channel>> activechannel_;
        activechannel_ = epoller_->poll();
        //std::cout << "polling" << std::endl;
        for(auto& it : activechannel_) {
            it->handleEvents(timerManager_);
        }
        handleExpired(); //will not timeout when reading or writing
        doPendingFunctors();
    }     
}

bool EventLoop::isloopInthisThread() const {
    return threadId_ == std::this_thread::get_id();
}; //can't reverse

void EventLoop::quit() { quit_ = true;};

EventLoop::~EventLoop() {
    loopInthisThread_ = nullptr;
}

void EventLoop::runInLoop(funcCallback&& cb) {
    if(isloopInthisThread()) {
        cb();
    }
    else {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(funcCallback&& cb) {
    {
        MutexLockGuard lock(mutex_);
        pendingfunctors_.push_back(std::move(cb)); //why task have to move?
    }
    if(!isloopInthisThread() || callingPendingFunctors_) {
        wakeup();
    }
}

int EventLoop::createEventFd() {
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        //fix bad syscall
    return fd;
}

void EventLoop::wakeup() { //where should i read?
    uint64_t spOffer = 1;
    ssize_t n = writen(wakeupFd_, &spOffer, sizeof spOffer);
}

void EventLoop::handleRead() {
    uint64_t spOffer = 1;
    ssize_t n = readn(wakeupFd_, &spOffer, sizeof spOffer); //figure out what's going on
    LOG_DEBUG << "handleRead res : " << n;
}

void EventLoop::doPendingFunctors() {
    std::vector<funcCallback> functors;
    callingPendingFunctors_ = true;
    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingfunctors_);
    }
    for(const auto& it : functors) {
        it();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::addtoPoller(std::shared_ptr<Channel> channel) {
    epoller_->epoll_add(channel);
}

void EventLoop::updatePoller(std::shared_ptr<Channel> channel) {
    epoller_->epoll_mod(channel);
}

void EventLoop::removeFromPoller(std::shared_ptr<Channel> channel) {
    epoller_->epoll_del(channel);
}

void EventLoop::handleExpired() { 
    timerManager_.handleExpiredTimer();
}

void EventLoop::addTimer(std::shared_ptr<Channel> channel, int TIMEOUT) {
    timerManager_.addTimer(channel, TIMEOUT);
}

void EventLoop::deleteTimer(std::shared_ptr<Channel> channel) {
    std::shared_ptr<TimerNode> timer = channel->getTimer();
    if(timer && !timer->isDeleting()) {
        timer->clearReq();
        timerManager_.deleteTimer(timer);
    }
}