#include "TimerNodeList.h"
#include "Timer.h"
#include "Channel.h"

TimerNodeList::TimerNodeList()
    :head_(new TimerNode),
     tail_(new TimerNode) {
         head_->next = tail_;
         head_->pre = tail_;
         tail_->next = head_;
         tail_->pre = head_;
    }

TimerNodeList::~TimerNodeList() {}

void TimerNodeList::addTimer(std::shared_ptr<Channel> channel, int timeout) {
    std::shared_ptr<TimerNode> newTimerNode(new TimerNode(channel, timeout));
    channel->linkTimer(newTimerNode);

    std::shared_ptr<TimerNode> preTmp = tail_->pre;
    preTmp->next = newTimerNode;
    newTimerNode->pre = preTmp;
    newTimerNode->next = tail_;
    tail_->pre = newTimerNode;
}

void TimerNodeList::deleteTimer(std::shared_ptr<TimerNode> timer) {
    std::shared_ptr<TimerNode> preTmp = timer->pre;
    std::shared_ptr<TimerNode> nextTmp = timer->next;
    preTmp->next = nextTmp;
    nextTmp->pre = preTmp;
}

void TimerNodeList::modTimer(std::shared_ptr<TimerNode> timer, int timeout) {
    //modTimer = deleteTimer + addTimer;
    timer->update(timeout);
    if(timer->next == tail_) {
        return;
    }

    std::shared_ptr<TimerNode> preTmp = timer->pre; //delete timer
    std::shared_ptr<TimerNode> nextTmp = timer->next;
    preTmp->next = nextTmp;
    nextTmp->pre = preTmp;

    preTmp = tail_->pre; //add timer
    preTmp->next = timer;
    timer->pre = preTmp;
    timer->next = tail_;
    tail_->pre = timer;
}

void TimerNodeList::handleExpiredTimer() {
    while(head_->next != tail_) {
        if(!head_->next->isValid()) {
            deleteTimer(head_->next);
        }
        else {
            break;
        }
    }
}
