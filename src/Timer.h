#pragma once
#include <iostream>
#include <memory>
#include <queue>
#include <list>

class Channel;

class TimerNode {
public:
  TimerNode(std::shared_ptr<Channel> channel, int timeout);
  TimerNode();
  ~TimerNode();
  void update(int timeout); //update expire time
  bool isValid(); //is timeout?
  size_t getExpTime(); //return timeout
  bool isDeleting() const;
  /*
  void setDeleted();
  bool isDeleted();
  */
  void clearReq();
private:
  size_t expTime_;
  bool isDeleting_;
public:
  std::shared_ptr<TimerNode> pre;
  std::shared_ptr<TimerNode> next;
private:
  std::weak_ptr<Channel> channel_;  //must be weak_ptr
};
/*
struct TimerCmp {
  bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) {
    return a->getExpTime() > b->getExpTime();
  }
};

class TimerManager {
private:
  //std::priority_queue<std::shared_ptr<TimerNode>, std::deque<std::shared_ptr<TimerNode>>, TimerCmp> timerNodeQueue;
  //std::unordered_map<std::shared_ptr<TimerNode>, int> timerNodeMap;
  std::list<std::shared_ptr<TimerNode>> timerNodeList;
public:
  TimerManager();
  ~TimerManager();
  void addTimer(std::shared_ptr<Channel> channel, int timeout);
  void modTimer(std::shared_ptr<TimerNode> timerNode, int timeout);
  void handleExpiredEvent();
};
*/