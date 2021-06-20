#include <iostream>
#include "Timer.h"
#include <sys/time.h>
#include "Channel.h"

TimerNode::TimerNode(std::shared_ptr<Channel> channel, int timeout)
    :channel_(channel),
     isDeleted_(false) {
        struct timeval now; //ms
        gettimeofday(&now, nullptr);
        expTime_  = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
        //std::cout << "Timer construct" << std::endl;
    }

TimerNode::~TimerNode() {
    if(channel_.lock()) {
      //std::cout << "close" << std::endl;
      channel_.lock()->handleClose();
    }
    //std::cout << "Timer distruct" << std::endl;
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
    setDeleted();
    return false;
  }
}

size_t TimerNode::getExpTime() {
  return expTime_;
}

void TimerNode::setDeleted() {
  //std::cout << "setDelete" << std::endl;
  isDeleted_ = true;
}

bool TimerNode::isDeleted() {
  return isDeleted_;
}

void TimerNode::clearReq() {
  channel_.reset(); //unbind TimerNode->channel
  setDeleted();
}

TimerManager::TimerManager() {}

TimerManager::~TimerManager() {}

void TimerManager::addTimer(std::shared_ptr<Channel> channel, int timeout) {
  std::shared_ptr<TimerNode> new_node(new TimerNode(channel, timeout));
  timerNodeQueue.push(new_node);
  channel->linkTimer(new_node);
}

void TimerManager::modTimer(std::shared_ptr<TimerNode> timerNode,int timeout) {
  timerNode->update(timeout);
}

void TimerManager::handleExpiredEvent() {
  while (!timerNodeQueue.empty()) {
    std::shared_ptr<TimerNode> ptimer_now = timerNodeQueue.top();
    //std::cout << ptimer_now << timerNodeQueue.size() << std::endl;
    if(ptimer_now->isDeleted()) {
      timerNodeQueue.pop();
    }
    else if(!ptimer_now->isValid()) {
      timerNodeQueue.pop();
    }
    else {
      break;
    }
  }
}
