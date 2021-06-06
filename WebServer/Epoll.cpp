#include "Epoll.h"
#include "EventLoop.h"
#include "Channel.h"

#define TIMEOUT 10000
Epoll::Epoll(EventLoop* loop)
    :loop_(loop),
    epollfd_(epoll_create(EPOLL_CLOEXEC)), 
    events_(4096) { //warning: fix bug
    }

Epoll::~Epoll() {}

std::vector<std::shared_ptr<Channel>> Epoll::poll() {
    std::vector<std::shared_ptr<Channel>> activechannel_;
    std::cout << "poller : polling" << std::endl;
    int event_nums = epoll_wait(epollfd_, &*events_.begin(), events_.size(), -1);
    for(int i = 0; i < event_nums; ++i) {
        int fd = events_[i].data.fd;
        //std::cout << "Fd: " << fd <<std::endl;
        activechannel_.push_back(fd2chan_[fd]);
    }
    return activechannel_;
}

void Epoll::epoll_add(std::shared_ptr<Channel> channel) {
    int fd = channel->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = channel->getEvents();
    //event.events = EPOLLIN|EPOLLET;
    fd2chan_[fd] = channel; //connect fd-channel
    epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event);
    //std::cout << "epoll_ctl: " <<epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) << channel->getFd() << std::endl;
}

void Epoll::epoll_mod(std::shared_ptr<Channel> channel) {
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
    auto it = fd2chan_.begin();  //erase fd
    fd2chan_.find(fd);
    fd2chan_.erase(it);
    epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event);
}

std::shared_ptr<Channel> Epoll::getChannel(int fd) {
    return fd2chan_[fd];
}
