#pragma once
#include <iostream>
#include "EventLoopThreadPool.h"
#include "EventLoop.h"


int main(){
    EventLoop loop;
    EventLoopThreadPool pool(3, &loop);
    pool.start();
    sleep(60);
    return 0;
}