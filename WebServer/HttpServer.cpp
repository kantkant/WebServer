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

const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000;  //keep alive for 5 min

pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

void MimeType::init() {
    mime["index.html"] = "Kant's WebServer";
    mime["default"] = "400";
    mime["hello"] = "Hello, World!\n";
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

HttpServer::HttpServer(): 
    state_(STATE_PARSE_URI),
    method_(METHOD_GET),
    keepAlive_(false),
    resourceName_("index.html"),
    HTTPVersion_(HTTP_11),
    isReadAgain_(false),
    nowReadPos_(0),
    hState_(H_START) {
        //std::cout << "httpServer construct" << std::endl;
    }
HttpServer::~HttpServer() {
    //std::cout << "httpServer distruct" << std::endl;
}

void HttpServer::messageCallback(std::string &inbuffer, std::string &outBuffer) { //how to deal with data?
    inBuffer_ += inbuffer; //outbuffer_ clean in handleError
    //outBuffer = bufferData_;
    //httpConn_.lock()->enableWriting();
    //std::cout << inbuffer << std::endl;
    httpAnalysisRequest();
    if(state_ == STATE_FINISH) {
        //std::cout << "finish" << std::endl;
        analysisReset();
    } //handleError->handleWrite->writeCompleteCallback->shutdown;
    //std::cout << inBuffer_ << std::endl;
    //std::cout << "------------" << std::endl;
    //std::cout << isReadAgain_ << std::endl;
    if(!isReadAgain_ && httpConn_.lock()) {
        outBuffer += outBuffer_; //outbuffer outside should not be clear, inbuffer inside should not be clear
        outBuffer_.clear();
        httpConn_.lock()->handleWrite();
        //std::cout << outBuffer << std::endl;
        return;
    }

    //httpConn_.lock()->enableReading();
    isReadAgain_ = false;
}

void HttpServer::writeCompleteCallback() {
    if(httpConn_.lock() && (!keepAlive_ || httpConn_.lock()->connectionState_ == H_DISCONNECTING)) {
        //std::cout << "shut" << std::endl;
        httpConn_.lock()->shutDownInConn();
        //httpConn_.lock()->handleClose();
    }
}

void HttpServer::connectionCallback(std::shared_ptr<HttpConn> httpconn) {
    httpconn->setExpTime(DEFAULT_KEEP_ALIVE_TIME);
    httpConn_ = httpconn;
}
void HttpServer::closeCallback() {

}

void HttpServer::handleError(std::string errormsg, int errorcode) {
    //send in outBuffer;
    //std::cout << "hadleError" << std::endl;
    errormsg = " " + errormsg;
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
    outBuffer_.clear();
    outBuffer_ = header_buff + body_buff;
    if(httpConn_.lock()) {
        httpConn_.lock()->handleError();
    }
}

void HttpServer::httpAnalysisRequest() {
    if(state_ == STATE_PARSE_URI) {
        URIState flag = parseURI();
        if(flag == PARSE_URI_AGAIN) { //inbuffer_->requesetLine deleted
            isReadAgain_ = true;
            return;
        }
        if(flag == PARSE_URI_ERROR) {
            handleError();
            return;
        }
        state_ = STATE_PARSE_HEADERS;
    }
    if(state_ == STATE_PARSE_HEADERS) {
        HeaderState flag = parseHeaders();
        if(flag == PARSE_HEADER_AGAIN) {
            isReadAgain_ = true;
            return;
        }
        if(flag == PARSE_HEADER_ERROR) {
            handleError();
            return;
        }
    }
    state_ = (method_ == METHOD_POST) ? STATE_RECV_BODY : STATE_ANALYSIS; //post not used
    if(state_ == STATE_RECV_BODY) {
        int content_length = -1;
        if(headers_.find("Content-length") != headers_.end()) {
            content_length = stoi(headers_["Content-length"]);
        } 
        if(content_length == -1) {
            std::string errorMsg = "Bad Request: Lack of argument (Content-length)";
            handleError(errorMsg);
            return;
        }
        if(static_cast<int>(inBuffer_.size()) < content_length) { //WARNING : inbuffer_ should not be compared with length
            return;
        }
        state_ = STATE_ANALYSIS;
    }
    if(state_ == STATE_ANALYSIS) {
        AnalysisState flag = analysisRequest();
        if(flag != ANALYSIS_SUCCESS) {
            return;
        }
        state_ = STATE_FINISH;
    }
}
URIState HttpServer::parseURI() {
    // 读到完整的请求行再开始解析请求
    size_t pos = inBuffer_.find('\r', nowReadPos_);
    if(pos < 0) {
        return PARSE_URI_AGAIN;
    }
    // 去掉请求行所占的空间，节省空间
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
    pos = request_line.find("/", pos);
    if(pos < 0) {
        resourceName_ = "index.html";
        HTTPVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }
    size_t _pos = request_line.find(' ', pos); 
    if(_pos < 0) {                      //method *source version
        return PARSE_URI_ERROR;         //headers : key->value
    }                                   //body
    if(_pos - pos > 1) {
        resourceName_ = request_line.substr(pos + 1, _pos - pos - 1); //' 'xxx'?' 

    }
    size_t __pos = resourceName_.find('?');
    if(_pos - pos > 1 && __pos >= 0) {
        resourceName_ = resourceName_.substr(0, __pos); //[)-> 0 + __pos
    }
    if(_pos - pos <= 1) {
        resourceName_ = "index.html";
    }
    pos = _pos;
    // cout << "fileName_: " << fileName_ << endl;
    // HTTP 版本号
    pos = request_line.find("/", pos);
    if(pos < 0) {
        return PARSE_URI_ERROR;
    }
    if(request_line.size() - pos <= 3) { //1.0 or 1.1
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

AnalysisState HttpServer:: analysisRequest() {  //to do : 20210619
    if(method_ == METHOD_POST) {
        //todo
    }
    else if(method_ == METHOD_GET || method_ == METHOD_HEAD) {
        std::string header;
        header += "HTTP/1.1 200 OK\r\n";
        if(headers_.find("Connection") != headers_.end() && (headers_["Connection"] == "Keep-Alive" || headers_["Connection"] == "keep-alive")) {
            keepAlive_ = true;
            //std::cout << "keepalive" << keepAlive_ << std::endl;
            header += std::string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + std::to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        }
        //std::cout << resourceName_ << std::endl;
        std::string resourceContent = MimeType::getMime(resourceName_);
       // std::cout << resourceContent << std::endl;
        if(resourceContent == "400") {
            //std::cout << "400" << std::endl;
            handleError();
            return ANALYSIS_ERROR;
        }
        header += "Content-Length: " + std::to_string(static_cast<int>(resourceContent.size())) + "\r\n";
        header += "Server: Kant's Web Server\r\n";
        header += "\r\n";
        // 头部结束
        outBuffer_ += header;
        if(method_ == METHOD_HEAD) {
            return ANALYSIS_SUCCESS;
        }
        outBuffer_ += resourceContent;
        return ANALYSIS_SUCCESS;
    }
}

void HttpServer::analysisReset() {
    nowReadPos_ = 0;
    state_ = STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
}

HeaderState HttpServer::parseHeaders() {
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
                if(inBuffer_[i] != '\r' && i - value_start > 255) {
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
