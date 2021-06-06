#include "EventLoop.h"
#include <iostream>
#include <sys/syscall.h>
#include <thread>
#include <assert.h>
#include "Util.h"
#include "Channel.h"
#include "MutexLock.h"
#include "Condition.h"

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
        if(!loopInthisThread_) {loopInthisThread_ = this;}
        //Epoll* tmpepoll_ = new Epoll(this);
        wakeupChannel_->setEvents(EPOLLIN | EPOLLET);
        wakeupChannel_->setReadcallback(std::bind(&EventLoop::handleRead, this)); //watch out std::bind
        //epoller_->epoll_add(wakeupChannel_);
        //std::cout << wakeupChannel_->getEvents() << std::endl;
        addtoPoller(wakeupChannel_);
        std::cout << "Create Fd" << wakeupFd_ <<std::endl;
    }


void EventLoop::loop() {
    assert(isloopInthisThread());
    while(!quit_) {
        std::vector<std::shared_ptr<Channel>> activechannel_;
        activechannel_ = epoller_->poll();
        for(auto& it : activechannel_) {
            //std::cout << "loopId : " << this << std::endl;
            it->handleEvents();
            
        }
        doPendingFunctors();
    }     
}

bool EventLoop::isloopInthisThread() const {return threadId_ == std::this_thread::get_id();}; //can't reverse

void EventLoop::quit() { quit_ = true;};

EventLoop::~EventLoop() {
    loopInthisThread_ = nullptr;
}

void EventLoop::runInLoop(funcCallback&& cb) {
    if(isloopInthisThread()) {
        cb();
        //std::cout << "run1-runinloop" << std::endl;
    }
    else {
        queueInLoop(std::move(cb));
        //std::cout << "run2-runinloop" << std::endl;
    }
}

void EventLoop::queueInLoop(funcCallback&& cb) {
    {
        MutexLockGuard lock(mutex_);
        pendingfunctors_.push_back(std::move(cb)); //why task have to move?
    }
    //std::cout << "pendingfunctors: " << pendingfunctors_.size() << std::endl;
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
    //write(wakeupFd_, spOffer&spOffer, sizeof spOffer);
    std::cout << "EventLoop::wakeupFd_ :" << wakeupFd_ << std::endl; 
    //std::cout << "Events" << wakeupChannel_->getEvents() << std::endl;
    ssize_t n = writen(wakeupFd_, &spOffer, sizeof spOffer); //figure out what's going on
    std::cout << n << std::endl;
}

void EventLoop::handleRead() {
    uint64_t spOffer = 1;
    ssize_t n = readn(wakeupFd_, &spOffer, sizeof spOffer); //figure out what's going on
    //epoller_->epoll_mod(wakeupChannel_); //reset wakeupchannel_
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

    //std::cout << "dopendingfunctors: " << functors.size() << std::endl;
    //std::cout << this << "dopendingfunctors: " << functors.size() << std::endl;
}

void EventLoop::addtoPoller(std::shared_ptr<Channel> channel) {
    epoller_->epoll_add(channel);
}

void EventLoop::updatePoller(std::shared_ptr<Channel> channel) {
    epoller_->epoll_mod(channel);
}