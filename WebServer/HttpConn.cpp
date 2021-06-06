#include "HttpConn.h"
#include "Util.h"
#include <memory>

HttpConn::HttpConn(EventLoop* loop, int fd)
    :loop_(loop),
     fd_(fd),
     channel_(new Channel(loop_, fd_)) {
         //channel_->setHolder(shared_from_this()); //c++11
         channel_->setReadcallback(std::bind(&HttpConn::handleRead, this));
    }

HttpConn::~HttpConn() {};

void HttpConn::tie() {
    channel_->setHolder(shared_from_this()); //can not set in constructor
}


std::shared_ptr<Channel> HttpConn::getChannel() {
    return channel_;
}

void HttpConn::handleRead() {
    std::string inbuffer;
    int fd = getChannel()->getFd();
    int n = readn(fd, inbuffer);
    std::cout << inbuffer << std::endl;

}

void HttpConn::handleNewEvents() {
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->addtoPoller(channel_);
    
    std::cout << "new Events handle OK" << std::endl;
    /*
        FILE* fp = fopen("1.txt", "w");
    fprintf(fp, " ok");
    fclose(fp);
    */
}