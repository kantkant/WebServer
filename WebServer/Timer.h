#pragma once
#include <iostream>
#include <memory>
#include <queue>

class Channel;

class TimerNode {
public:
  TimerNode(std::shared_ptr<Channel> channel, int timeout);
  ~TimerNode();
  void update(int timeout); //update expire time
  bool isValid(); //is timeout?
  size_t getExpTime(); //return timeout
  void setDeleted();
  bool isDeleted();
  void clearReq();
private:
    size_t expTime_;
    bool isDeleted_;
    std::shared_ptr<Channel> channel_;
};

struct TimerCmp {
  bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) {
    return a->getExpTime() > b->getExpTime();
  }
};

class TimerManager {
private:
  std::priority_queue<std::shared_ptr<TimerNode>, std::deque<std::shared_ptr<TimerNode>>, TimerCmp> timerNodeQueue;
public:
  TimerManager();
  ~TimerManager();
  void addTimer(std::shared_ptr<Channel> channel, int timeout);
  void handleExpiredEvent();
};