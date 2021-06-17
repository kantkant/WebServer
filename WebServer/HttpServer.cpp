#include "HttpConn.h"
#include "Util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <memory>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/mman.h>


HttpServer::HttpServer() {}
HttpServer::~HttpServer() {}

void HttpServer::messageCallback(std::string &inbuffer, std::string &outBuffer_) { //how to deal with data?
    receiveData_ += inbuffer;
    if(receiveData_.size() > 0) {
        std::cout << inbuffer << std::endl;
        if(httpConn_.lock()) {
            httpConn_.lock()->enableWriting();
        }
        outBuffer_ = receiveData_;
        receiveData_.clear();
    }
}

void HttpServer::writeCompleteCallback() {
    std::cout << "writeComplete" << std::endl;
    if(httpConn_.lock()) {
        httpConn_.lock()->enableReading();
    }
}

void HttpServer::connectionCallback(std::shared_ptr<HttpConn> httpconn) {
    httpConn_ = httpconn;
}
void HttpServer::closeCallback() {}

void errorCallback(int fd, int errorcode, std::string &errormsg) {}
/*
const int DEFAULT_EXPIRED_TIME = 1; //2s telnet default shortConn
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000; //30s  browser defalut keep-alive

pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

void MimeType::init() {
    mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/msword";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "image/x-icon";
    mime[".jpg"] = "image/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix) {
    pthread_once(&once_control, MimeType::init);
    if(mime.find(suffix) == mime.end()) {
        return mime["default"];
    }
    else {
        return mime[suffix];
    }
}

HttpConn::HttpConn(EventLoop* loop, int fd)
    :loop_(loop),
     fd_(fd),
     channel_(new Channel(loop_, fd_)),
     keepAlive_(false),
     connectionState_(H_CONNECTED),
     method_(METHOD_GET),
     HTTPVersion_(HTTP_11),
     state_(STATE_PARSE_URI),
     nowReadPos_(0),
     error_(false) {
         channel_->setReadcallback(std::bind(&HttpConn::handleRead, this));
         channel_->setWritecallback(std::bind(&HttpConn::handleWrite, this));
         channel_->setCloseCallBack(std::bind(&HttpConn::handleClose, this));
        // std::cout << "httpConn construct" << std::endl;
    }

HttpConn::~HttpConn() {};//std::cout << "httpConn distruct" << std::endl;

void HttpConn::tie() {
    channel_->setHolder(shared_from_this()); //can not set in constructor
}

std::shared_ptr<Channel> HttpConn::getChannel() {
    return channel_;
}

void HttpConn::handleRead() {
    httpAnalysisRequest(); //what if error?
    //std::cout << "handleRead" << std::endl;
    if(error_) {
        handleClose();
    }
    if(!error_ && state_ == STATE_FINISH) {
        //std::cout << "finish" << std::endl;
        reSet();
    }
    if(!error_ && outBuffer_.size() > 0) {
        channel_->setEvents(EPOLLOUT | EPOLLET);
        if(timer_.lock()) {
            std::shared_ptr<TimerNode> my_timer(timer_.lock());
            my_timer->clearReq();
            timer_.reset();
        }
        if(keepAlive_) {
            loop_->updatePoller(channel_, DEFAULT_KEEP_ALIVE_TIME);
        }
        else{
            loop_->updatePoller(channel_, DEFAULT_EXPIRED_TIME);
        }
        return;
    }
}

void HttpConn::handleWrite() {
    if(error_) {
        handleClose();
    }
    int writeNum = writen(fd_, outBuffer_);        // kernel  appl
    if(!error_ && connectionState_ != H_DISCONNECTED && writeNum < 0) {
       // std::cout << "handleWrite" << std::endl;
        channel_->setEvents(0);
        error_ = true;
    }
    if(!error_ && connectionState_ != H_DISCONNECTED && outBuffer_.size() == 0) {
        //std::cout << "handleWrite" << std::endl;
        channel_->setEvents(EPOLLIN | EPOLLET);
    }
    if(timer_.lock()) {
        std::shared_ptr<TimerNode> my_timer(timer_.lock());
        my_timer->clearReq();
        timer_.reset();
    }
    if(keepAlive_) { //is necessary?
        loop_->updatePoller(channel_, DEFAULT_KEEP_ALIVE_TIME);
    }
    else{
        loop_->updatePoller(channel_, DEFAULT_EXPIRED_TIME);
    }
}

void HttpConn::handleNewEvents() {
    channel_->setEvents(EPOLLIN | EPOLLET);
    loop_->addtoPoller(channel_, DEFAULT_EXPIRED_TIME);
    //std::cout << "new Events handle OK" << std::endl;
}

void HttpConn::handleClose() {
    connectionState_ = H_DISCONNECTED;
    std::shared_ptr<HttpConn> guard(shared_from_this());
    if(timer_.lock()) {
        std::shared_ptr<TimerNode> my_timer(timer_.lock());
        my_timer->clearReq();
        timer_.reset();
    }
    loop_->removeFromPoller(channel_);
    close(fd_);
    //shutdown(fd_, SHUT_WR);
}

void HttpConn::handleError(int fd, int errorcode, std::string errormsg) {
    errormsg = " " + errormsg;
    char send_buff[4096];
    std::string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += std::to_string(errorcode) + errormsg;
    body_buff += "<hr><em> Kant's Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + std::to_string(errorcode) + errormsg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: Kant's Web Server\r\n";
  
    header_buff += "\r\n";
  // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}

void HttpConn::linkTimer(std::shared_ptr<TimerNode> timernode) {
    timer_ = timernode;
}

URIState HttpConn::parseURI() {
    std::string cop = inBuffer_;
    // 读到完整的请求行再开始解析请求
    size_t pos = inBuffer_.find('\r', nowReadPos_);
    if(pos < 0) {
        return PARSE_URI_AGAIN;
    }
    // 去掉请求行所占的空间，节省空间  //why?
    std::string request_line = inBuffer_.substr(0, pos);
    if(inBuffer_.size() > pos + 1) {
        inBuffer_ = inBuffer_.substr(pos + 1); // ....\r ....
    }
    else {
        inBuffer_.clear();
    }
    //Method
    int posGet = request_line.find("GET");
    int posPost = request_line.find("POST");
    int posHead = request_line.find("HEAD");
    if(posGet >= 0) {
        pos = posGet;
        method_ = METHOD_GET;
    }
    else if(posPost >= 0) {
        pos = posPost;
        method_ = METHOD_POST;
    }
    else if(posHead >= 0) {
        pos = posHead;
        method_ = METHOD_HEAD;
    }
    else {
        return PARSE_URI_ERROR;
    }
    //filename
    pos = request_line.find("/", pos);
    if(pos < 0) {
        fileName_ = "index.html";
        HTTPVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }
    size_t _pos = request_line.find(' ', pos); 
    size_t __pos = fileName_.find('?');
    if(_pos < 0) {
        return PARSE_URI_ERROR;
    }
    if(_pos - pos > 1) {
        fileName_ = request_line.substr(pos + 1, _pos - pos - 1);

    }
    if(_pos - pos > 1 && __pos >= 0) {
        fileName_ = fileName_.substr(0, __pos);
    }
    if(_pos - pos <= 1) {
        fileName_ = "index.html";
    }
    pos = _pos;
    // cout << "fileName_: " << fileName_ << endl;
    // HTTP 版本号
    pos = request_line.find("/", pos);
    if(pos < 0) {
        return PARSE_URI_ERROR;
    }
    if(request_line.size() - pos <= 3) {
        return PARSE_URI_ERROR;
    }
    std::string ver = request_line.substr(pos + 1, 3);
    if(ver == "1.0") {
        HTTPVersion_ = HTTP_10;
    }
    if(ver == "1.1") {
        HTTPVersion_ = HTTP_11;
    }
    if(ver != "1.0" && ver != "1.1") {
        return PARSE_URI_ERROR;
    }
    return PARSE_URI_SUCCESS;
}

HeaderState HttpConn::parseHeaders() {
    int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    size_t i = 0;
    for (; i < inBuffer_.size() && notFinish; ++i) {
        switch (hState_) {
            case H_START: {
                if(inBuffer_[i] == '\n' || inBuffer_[i] == '\r') { 
                    break;
                }
                hState_ = H_KEY;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
            case H_KEY: {
                if(inBuffer_[i] == ':') {
                    key_end = i;
                    if(key_end - key_start <= 0) {
                        return PARSE_HEADER_ERROR;
                    }
                    hState_ = H_COLON;
                }
                if(inBuffer_[i] == '\n' || inBuffer_[i] == '\r') {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_COLON: {
                if(inBuffer_[i] == ' ') {
                    hState_ = H_SPACES_AFTER_COLON;
                } 
                else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_SPACES_AFTER_COLON: {
                hState_ = H_VALUE;
                value_start = i;
                break;
            }
            case H_VALUE: {
                if(inBuffer_[i] == '\r') {
                    hState_ = H_CR;
                    value_end = i;
                } 
                if(inBuffer_[i] == '\r' && value_end - value_start <= 0) {
                    return PARSE_HEADER_ERROR;
                }
                if(i - value_start > 255) {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_CR: {
                if(inBuffer_[i] == '\n') {
                    hState_ = H_LF;
                    std::string key(inBuffer_.begin() + key_start, inBuffer_.begin() + key_end);
                    std::string value(inBuffer_.begin() + value_start, inBuffer_.begin() + value_end);
                    headers_[key] = value;
                    now_read_line_begin = i;
                } 
                else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_LF: {
                if (inBuffer_[i] == '\r') {
                    hState_ = H_END_CR;
                } 
                else {
                    key_start = i;
                    hState_ = H_KEY;
                }
                break;
            }
            case H_END_CR: {
                if (inBuffer_[i] == '\n') {
                    hState_ = H_END_LF;
                } 
                else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_END_LF: {
                notFinish = false;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
        }
    }
    if(hState_ == H_END_LF) {
        inBuffer_ = inBuffer_.substr(i);
        return PARSE_HEADER_SUCCESS;
    }
    inBuffer_ = inBuffer_.substr(now_read_line_begin);
    return PARSE_HEADER_AGAIN;
}

AnalysisState HttpConn::analysisRequest() {  //outbuffer write
    if(method_ == METHOD_POST) {
        //todo
    }
    else if(method_ == METHOD_GET || method_ == METHOD_HEAD) {
        std::string header;
        header += "HTTP/1.1 200 OK\r\n";
        if(headers_.find("Connection") != headers_.end() && (headers_["Connection"] == "Keep-Alive" || headers_["Connection"] == "keep-alive")) {
            keepAlive_ = true;
            header += std::string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + std::to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        }
        int dot_pos = fileName_.find('.');
        std::string filetype;
        if(dot_pos < 0) {
            filetype = MimeType::getMime("default");
        }
        else {
            filetype = MimeType::getMime(fileName_.substr(dot_pos));
        }

        // echo test
        if(fileName_ == "hello") {
            outBuffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-Length: 11\r\n\r\nHello World\n"; //browser analysis needs content-length
            state_ == STATE_FINISH;
            return ANALYSIS_SUCCESS;
        }

        if(fileName_ == "favicon.ico") {
            header += "Content-Type: image/png\r\n";
            header += "Content-Length: " + std::to_string(sizeof favicon_) + "\r\n";
            header += "Server: Kant's Web Server\r\n";
            header += "\r\n";
            outBuffer_ += header;
            outBuffer_ += std::string(favicon_, favicon_ + sizeof favicon_);
            state_ == STATE_FINISH;
            return ANALYSIS_SUCCESS;
        }
        struct stat sbuf;
        if(stat(fileName_.c_str(), &sbuf) < 0) {
            header.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        header += "Content-Type: " + filetype + "\r\n";
        header += "Content-Length: " + std::to_string(sbuf.st_size) + "\r\n";
        header += "Server: Kant's Web Server\r\n";
        header += "\r\n";
        // 头部结束
        outBuffer_ += header;
        if(method_ == METHOD_HEAD) {
            return ANALYSIS_SUCCESS;
        }
        int src_fd = open(fileName_.c_str(), O_RDONLY, 0);
        if(src_fd < 0) {
            outBuffer_.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        close(src_fd);
        if(mmapRet == (void *)-1) {
            munmap(mmapRet, sbuf.st_size);
            outBuffer_.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        char *src_addr = static_cast<char *>(mmapRet);
        outBuffer_ += std::string(src_addr, src_addr + sbuf.st_size);
        munmap(mmapRet, sbuf.st_size);
        return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}

void HttpConn::httpAnalysisRequest() {
    bool zero = false;
    int read_num = readn(fd_, inBuffer_, zero);
    if(connectionState_ == H_DISCONNECTING) {
        inBuffer_.clear();
        return;
    }
    if(read_num < 0) {
        error_ = true;
        handleError(fd_, 400, "Bad Request");
        return;
    }
    if(zero) {
        connectionState_ = H_DISCONNECTING; //distinct read_num = 0 and read_num != 0
        if (read_num == 0) {
            return;
        }
    }
    if(state_ == STATE_PARSE_URI) {
        URIState flag = parseURI();
        if(flag == PARSE_URI_AGAIN) {
            return;
        }
        else if(flag == PARSE_URI_ERROR) {
            inBuffer_.clear();
            error_ = true;
            handleError(fd_, 400, "Bad Request");
            return;
        }
        else {
            state_ = STATE_PARSE_HEADERS;
        }
    }
    if(state_ == STATE_PARSE_HEADERS) {
        HeaderState flag = parseHeaders();
        if(flag == PARSE_HEADER_AGAIN) {
            return;
        }
        if(flag == PARSE_HEADER_ERROR) {
            error_ = true;
            handleError(fd_, 400, "Bad Request");
            return;
        }
        if(method_ == METHOD_POST) { //post not used
        // POST方法准备
            state_ = STATE_RECV_BODY;
        } 
        else {
            state_ = STATE_ANALYSIS;
        }
    }
    if(state_ == STATE_RECV_BODY) {
        int content_length = -1;
        if(headers_.find("Content-length") != headers_.end()) {
            content_length = stoi(headers_["Content-length"]);
        } 
        else {
            error_ = true;
            handleError(fd_, 400, "Bad Request: Lack of argument (Content-length)");
            return;
        }
        if(static_cast<int>(inBuffer_.size()) < content_length) {
            return;
        }
        state_ = STATE_ANALYSIS;
    }
    if(state_ == STATE_ANALYSIS) {
        AnalysisState flag = analysisRequest();
        if(flag == ANALYSIS_SUCCESS) {
            state_ = STATE_FINISH;
            return;
        }
        else {
            error_ = true;
            return;
        }
    }
}

void HttpConn::reSet() {
    fileName_.clear();
    nowReadPos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
}
*/