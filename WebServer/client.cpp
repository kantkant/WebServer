#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include "noncopyable.h"
#include "MutexLock.h"

class Client : noncopyable {
private:
    Client();
    ~Client();
    static Client* instance_;
    static MutexLock mt;
    std::string inbuffer_;
    std::string outbuffer_;
    int fd;
public:
    static Client* Singleton();
    void callServer();
};

MutexLock Client::mt;

Client::Client() {};
Client::~Client() {};

Client* Client::instance_ = nullptr;

Client* Client::Singleton() {
    if(instance_ == 0) {
        MutexLockGuard lock(mt);
        if(instance_ == 0) {
            instance_ = new Client();
            return instance_;
        }
    }
}

void Client::callServer() {
    struct sockaddr_in serv_addr;
    fd = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);
    connect(fd, (struct sockaddr*) &serv_addr, sizeof serv_addr);
    while(std::cin >> outbuffer_) {
        write(fd, &outbuffer_, outbuffer_.size());
        outbuffer_.clear();
        //read(fd, &inbuffer_, inbuffer_.size());
        //std::cout << inbuffer_ << std::endl;
        //inbuffer_.clear();
    }
}

int main() {
    Client* p = Client::Singleton();
    p->callServer();
    sleep(1000);
    return 0;
}

