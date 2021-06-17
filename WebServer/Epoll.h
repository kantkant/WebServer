#pragma once
#include <iostream>
#include "noncopyable.h"
#include <vector>
#include <sys/epoll.h>
#include "EventLoop.h"
#include <sys/socket.h>
#include <map>
#include "Timer.h"

class EventLoop; //fix bug
class Channel;
class HttpConn;

class Epoll : noncopyable {
public:
    Epoll(EventLoop* loop);
    ~Epoll();
    void epoll_add(std::shared_ptr<Channel> channel);
    void epoll_mod(std::shared_ptr<Channel> channel);
    void epoll_del(std::shared_ptr<Channel> channel);
    std::vector<std::shared_ptr<Channel>> poll();
    std::shared_ptr<Channel> getChannel(int fd); //avoid shared_from_this
    //void removeFromPoller(std::shared_ptr<Channel> channel);
    void setHttpConn(std::shared_ptr<HttpConn> httpconn, int fd);
private:
    int epollfd_;
    std::vector<epoll_event> events_;
    std::map<int, std::shared_ptr<Channel>> fd2chan_;
    std::map<int, std::shared_ptr<HttpConn>> fd2http_;
    EventLoop* loop_;
};