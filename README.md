# A C++ High Performance Web Server
  
## Introduction  

本项目为C++11编写的Web服务器，解析了get、head请求，可处理静态资源，支持HTTP长连接，支持管线化请求，记录服务器运行状态。  

测试页：http://www.neijuanwang.com/


| Part Ⅰ | Part Ⅱ | Part Ⅲ | Part Ⅳ |
| :--------: | :---------: | :---------: | :---------: |
| [并发模型](https://github.com/kantkant/WebServer/并发模型.md)|[连接的维护](https://github.com/kantkant/WebServer/连接的维护.md) | [测试及改进](https://github.com/kantkant/WebServer/测试及改进.md) | [项目目的](https://github.com/kantkant/WebServer/项目目的.md)
## Envoirment  
* OS: Ubuntu 18.04
* Complier: g++ 4.8

## Build

	./build.sh

## Usage

	./myWebServer

## Technical points
* 使用Epoll边沿触发的IO多路复用技术，非阻塞IO，使用Reactor模式
* 使用多线程充分利用多核CPU，并使用线程池避免线程频繁创建销毁的开销
* 使用基于小根堆的定时器关闭超时请求
* 主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程(兼计算线程)，锁的争用只会出现在主线程和某一特定线程中
* 使用eventfd实现了线程的异步唤醒
* 为减少内存泄漏的可能，使用智能指针等RAII机制
* 使用状态机解析了HTTP请求,支持管线化
* 支持优雅关闭连接
 
## Model

并发模型为Reactor+非阻塞IO+线程池，新连接Round Robin分配，详细介绍请参考[并发模型](https://github.com/kantkant/WebServer/并发模型.md)
![并发模型](https://github.com/kantkant/WebServer/testData/model.png)

## 代码统计

![cloc](https://github.com/kantkant/WebServer/testData/cloc.png)



