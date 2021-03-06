# A C++ High Performance Web Server
  
## Introduction  

本项目为C++11编写的Web服务器，解析了get、head请求，可处理静态资源，支持HTTP长连接，以守护进程的方式运行。  

测试页：http://www.neijuanwang.com/main


| Part Ⅰ | Part Ⅱ | Part Ⅲ | Part Ⅳ | 
| :--------: | :---------: | :---------: | :---------: |
| [并发模型](https://github.com/kantkant/WebServer/blob/master/并发模型.md)|[连接的维护](https://github.com/kantkant/WebServer/blob/master/连接的维护.md) | [测试、改进及部署](https://github.com/kantkant/WebServer/blob/master/测试、改进及部署.md) | [遇到的困难](https://github.com/kantkant/WebServer/blob/master/遇到的困难.md)
## Envoirment  
* OS: Ubuntu 18.04
* Complier: g++ 4.8

## Build

	./build.sh

## Usage

	./myWebServerd

## Technical points
* 使用组合代替继承，减少代码耦合，降低重构成本
* 基于epoll的IO复用机制实现Reactor模式，采用边缘触发模式和非阻塞模式
* 主线程只负责accept请求，并以Round Robin的方式分发给其它IO线程(兼计算线程)，锁的争用只会出现在主线程和某一特定线程中
* 使用eventfd实现了线程的异步唤醒
* 为减少内存泄漏的可能，使用智能指针等RAII机制
* 支持优雅关闭连接
 
## Model

并发模型为Reactor+非阻塞IO+线程池，新连接Round Robin分配，详细介绍请参考[并发模型](https://github.com/kantkant/WebServer/blob/master/并发模型.md)
![并发模型](https://github.com/kantkant/WebServer/blob/master/testData/model.png)

## 代码统计

![cloc](https://github.com/kantkant/WebServer/blob/master/testData/cloc.png)



