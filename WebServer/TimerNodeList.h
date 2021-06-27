#pragma once
#include <iostream>
#include <memory>
#include "Timer.h"


class TimerNodeList {
public:
    TimerNodeList();
    ~TimerNodeList();
    void addTimer(std::shared_ptr<Channel> channel, int timeout);
    void deleteTimer(std::shared_ptr<TimerNode> timer);
    void modTimer(std::shared_ptr<TimerNode> imer, int timeout);
    void handleExpiredTimer();
private:
    std::shared_ptr<TimerNode> head_;
    std::shared_ptr<TimerNode> tail_;
};