#include <iostream>
#include "Timer.h"
#include <sys/time.h>
#include "Channel.h"

TimerNode::TimerNode(std::shared_ptr<Channel> channel, int timeout)
    :channel_(channel),
     isDeleting_(false),
     pre(nullptr),
     next(nullptr) {
        struct timeval now; //ms
        gettimeofday(&now, nullptr);
        expTime_  = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
        //std::cout << "Timer construct" << std::endl;
    }

TimerNode::TimerNode() {};

TimerNode::~TimerNode() {
  isDeleting_ = true;
  if(channel_.lock()) {
      //std::cout << "close" << std::endl;
  channel_.lock()->handleClose();
}
    //std::cout << "Timer distruct" << std::endl;
}

bool TimerNode::isDeleting() const {
  return isDeleting_;
}

void TimerNode::update(int timeout) {
  struct timeval now;
  gettimeofday(&now, NULL);
  expTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid() {
  struct timeval now;
  gettimeofday(&now, NULL);
  size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
  if(temp < expTime_){
    return true;
  }
  else {
    //setDeleted();
    return false;
  }
}

size_t TimerNode::getExpTime() {
  return expTime_;
}
/*
void TimerNode::setDeleted() {
  //std::cout << "setDelete" << std::endl;
  isDeleted_ = true;
}

bool TimerNode::isDeleted() {
  return isDeleted_;
}
*/
void TimerNode::clearReq() {
  channel_.reset(); //unbind TimerNode->channel
  //setDeleted();
}
/*
TimerManager::TimerManager() {}

TimerManager::~TimerManager() {}

void TimerManager::addTimer(std::shared_ptr<Channel> channel, int timeout) {
  std::shared_ptr<TimerNode> new_node(new TimerNode(channel, timeout));
  timerNodeList.push_back(new_node);
  channel->linkTimer(new_node);
}

void TimerManager::modTimer(std::shared_ptr<TimerNode> timerNode,int timeout) {
  timerNode->update(timeout);

}

void TimerManager::handleExpiredEvent() {
  while (!timerNodeList.empty()) {
    std::shared_ptr<TimerNode> ptimer_now = timerNodeList.front();
    //std::cout << ptimer_now << timerNodeQueue.size() << std::endl;
    if(ptimer_now->isDeleted()) {
      timerNodeList.pop_front();
    }
    else if(!ptimer_now->isValid()) {
      timerNodeList.pop_front();
    }
    else {
      break;
    }
  }
}
*/