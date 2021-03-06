#include "HttpConn.h"
#include "./base/Util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>
#include "HttpServer.h"

HttpConn::HttpConn(EventLoop* loop, int fd)
    :loop_(loop),
     channel_(new Channel(loop_, fd)),
     connectionState_(H_CONNECTED),
     httpServer_(new HttpServer()),
     isWriting_(false),
     closeInWrite_(false) {
         channel_->setReadcallback(std::bind(&HttpConn::handleRead, this));
         channel_->setWritecallback(std::bind(&HttpConn::handleWrite, this));
         channel_->setCloseCallBack(std::bind(&HttpConn::handleClose, this));
         //std::cout << "httpConn construct" << std::endl;
    }

HttpConn::~HttpConn() {
    //std::cout << "httpConn distruct" << std::endl;
}

void HttpConn::enableWriting() {
    channel_->setEvents(EPOLLOUT | EPOLLET);
    isWriting_ = true;
    loop_->updatePoller(channel_);
}

void HttpConn::disableWriting() {
    channel_->setEvents(EPOLLIN | EPOLLET);
    isWriting_ = false;
    loop_->updatePoller(channel_);
}

void HttpConn::handleRead() {
    inBuffer_.clear();
    bool isFIN = false;
    ssize_t n = readn(channel_->getFd(), inBuffer_, isFIN); //let channel get fd;
    //std::cout << "----------------------------" << std::endl;
    //std::cout << inBuffer_ << std::endl;
    //std::cout << "----------------------------" << std::endl;
    //std::cout << "handleRead" << std::endl;
    if(n > 0) {
        //std::cout << "MessegeComplete" << std::endl;
        httpServer_->messageCallback(inBuffer_, outBuffer_); //set TIME_OUT for one conn
    }
    else if(n == 0 && isFIN && outBuffer_.size() == 0) { //WARNING:keepalive readn = 0 ------------------- 06/21
        //std::cout << "handleClose" << std::endl;
        handleClose();
    }
    else if(n == 0 && isFIN && outBuffer_.size() != 0) { //if client send FIN, we should keep writing
        closeInWrite_ = true;
        handleWrite();
    }
    else {
        //RST
        handleClose();
    }
}

void HttpConn::handleWrite() { //care about buffer free
    int outBufSize = outBuffer_.size();
    //std::cout << outBuffer_ << std::endl;
    ssize_t n = writen(channel_->getFd(), outBuffer_);
    //std::cout << "handleWrite" << std::endl;
    if(n > 0 && n == outBufSize) {
        //std::cout << "WriteComp" << std::endl;
        httpServer_->writeCompleteCallback();  //watch out !!!!! 6/18
    }
    if(n >= 0 && n == outBufSize && (!httpServer_->isKeepAilve() || connectionState_ == H_DISCONNECTING || closeInWrite_)) {
        handleClose();   //shutdown when finish write
        //shutDownInConn();
        /*
        ????????????????????????????????????????????????????????????????????? shutDownInConn???
        ??????????????????????????????????????????????????????????????????????????????
        ??????shutDownInConn?????????????????????????????????????????????????????????????????????????????????
        ???????????????handleColse()??????????????????????????????
        */
        //std::cout << "shutdown" << std::endl;
        return;
    }
    if(n >= 0 && n == outBufSize && !closeInWrite_ && isWriting_) {
        disableWriting();
    }
    if(n >= 0 && n < outBufSize && !isWriting_) { //outbufsize != 0, but n->[0, outbufsize]
        enableWriting();
    }
    if(n < 0) {
        //error
        handleClose();
    }
}

void HttpConn::handleNewEvents() {
    connectionState_ = H_CONNECTED;
    channel_->setHolder(shared_from_this());
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->addtoPoller(channel_);
    httpServer_->connectionCallback(shared_from_this());
    loop_->addTimer(channel_, channel_->getExpTime());
}

void HttpConn::handleClose() {
    //std::cout << "close" << std::endl;
    connectionState_ = H_DISCONNECTED;
    std::shared_ptr<HttpConn> guard(shared_from_this());
    //channel_->untieTimer();
    loop_->deleteTimer(channel_);
    loop_->removeFromPoller(channel_);
    httpServer_->closeCallback();
    //shutdown(channel_->getFd(), SHUT_WR);
}

void HttpConn::handleError() { //return 400 request
    connectionState_ = H_DISCONNECTING;
    //conn->server : server->conn;
    //send in outBuffer_
}

void HttpConn::setExpTime(int timeout) {
    channel_->setExpTime(timeout);
}

void HttpConn::shutDownInConn() {
    shutDownWR(channel_->getFd());
}