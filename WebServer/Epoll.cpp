#include "Epoll.h"
#include "EventLoop.h"
#include "Channel.h"

#define TIMEOUT 5000
Epoll::Epoll(EventLoop* loop)
    :loop_(loop),
    epollfd_(epoll_create(EPOLL_CLOEXEC)), 
    events_(4096) { //warning: fix bug
    }

Epoll::~Epoll() {}

std::vector<std::shared_ptr<Channel>> Epoll::poll() {
    std::vector<std::shared_ptr<Channel>> activechannel_;
    //std::cout << "poller : polling" << std::endl;
    int event_nums = epoll_wait(epollfd_, &*events_.begin(), events_.size(), -1);
    for(int i = 0; i < event_nums; ++i) {
        int fd = events_[i].data.fd;
        activechannel_.push_back(fd2chan_[fd]);
    }
    return activechannel_;
}

void Epoll::epoll_add(std::shared_ptr<Channel> channel, int timeout) {
    if(timeout > 0) {
        addTimer(channel, timeout);
    }
    int fd = channel->getFd();
    fd2chan_[fd] = channel; //connect fd-channel
    struct epoll_event event;
    event.data.fd = fd;
    event.events = channel->getEvents();
    //event.events = EPOLLIN|EPOLLET;
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);
    //std::cout << "epoll_ctl: " <<epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) << channel->getFd() << std::endl;
}

void Epoll::epoll_mod(std::shared_ptr<Channel> channel, int timeout) {
    if(timeout > 0) {
        addTimer(channel, timeout);
    }
    int fd = channel->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    //event.events = EPOLLIN|EPOLLET;
    event.events = channel->getEvents();
    epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event);
}

void Epoll::epoll_del(std::shared_ptr<Channel> channel) {
    int fd = channel->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = channel->getEvents();
    epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event);
    auto itchan = fd2chan_.find(fd);  //erase fd
    fd2chan_.erase(itchan);
    auto ithttp = fd2http_.find(fd);
    fd2http_.erase(ithttp);
}

std::shared_ptr<Channel> Epoll::getChannel(int fd) {
    return fd2chan_[fd];
}

void Epoll::setHttpConn(std::shared_ptr<HttpConn> httpconn, int fd) {
    fd2http_[fd] = httpconn;
}

void Epoll::addTimer(std::shared_ptr<Channel> channel, int timeout) {
    std::shared_ptr<HttpConn> httpconn = channel->getHolder();
    if(httpconn) {
        timerManager_.addTimer(httpconn, timeout);
    }
    else {
        //bad httpconn;
    }
}

void Epoll::handleExpired() { 
    timerManager_.handleExpiredEvent();
}

