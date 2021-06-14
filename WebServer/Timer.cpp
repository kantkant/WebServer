#include <iostream>
#include "Timer.h"
#include <sys/time.h>
#include "HttpConn.h"

TimerNode::TimerNode(std::shared_ptr<HttpConn> httpconn, int timeout)
    :httpConn_(httpconn),
     isDeleted_(false) {
        struct timeval now; //ms
        gettimeofday(&now, nullptr);
        expTime_  = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
    }

TimerNode::~TimerNode() {
    if(httpConn_) {
        httpConn_->handleClose();
    }
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
  isDeleted_ = true;
}

bool TimerNode::isDeleted() {
  return isDeleted_;
}

void TimerNode::clearReq() {
  httpConn_.reset(); //unbind httpConn->TimerNode
  setDeleted();
}

TimerManager::TimerManager() {}

TimerManager::~TimerManager() {}

void TimerManager::addTimer(std::shared_ptr<HttpConn> httpconn, int timeout) {
  std::shared_ptr<TimerNode> new_node(new TimerNode(httpconn, timeout));
  timerNodeQueue.push(new_node);
  httpconn->linkTimer(new_node);
}

void TimerManager::handleExpiredEvent() {
  while (!timerNodeQueue.empty()) {
    std::shared_ptr<TimerNode> ptimer_now = timerNodeQueue.top();
    if(ptimer_now->isDeleted()) {
        timerNodeQueue.pop();
    }
    else if(ptimer_now->isValid() == false) {
        timerNodeQueue.pop();
    }
    else {
      break;
    }
  }
}