#pragma once
#include <iostream>
#include "EventLoop.h"
#include "Channel.h"
#include "Timer.h"
#include <memory>
#include <unordered_map>
#include "HttpConn.h"

enum ProcessState {
  STATE_PARSE_URI = 1,      //method:post,get,head -> uri:file path ->http version:1.0,1.1 
  STATE_PARSE_HEADERS,      //keepalive only
  STATE_RECV_BODY,          //not used yet
  STATE_ANALYSIS,           //get, head
  STATE_FINISH              
};

enum URIState {
  PARSE_URI_AGAIN = 1,      //wait for tcp package
  PARSE_URI_ERROR,          //bad request uri
  PARSE_URI_SUCCESS,        
};

enum HeaderState {
  PARSE_HEADER_SUCCESS = 1,
  PARSE_HEADER_AGAIN,       //wait for tcp package
  PARSE_HEADER_ERROR        //bad request header
};

enum AnalysisState {
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum HttpMethod {
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD 
};

enum HttpVersion {
    HTTP_10 = 1,
    HTTP_11
};

enum ParseState {
  H_START = 0,
  H_KEY,
  H_COLON,
  H_SPACES_AFTER_COLON,
  H_VALUE,
  H_CR,
  H_LF,
  H_END_CR,
  H_END_LF
};

class MimeType { //singleton
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType &m);
    ~MimeType();
public:
    static std::string getMime(const std::string &suffix);
private:
    static pthread_once_t once_control;
};

class HttpServer : noncopyable {
public:
    HttpServer();
    ~HttpServer();
    void writeCompleteCallback();
    void messageCallback(std::string &inbuffer, std::string &outBuffer);
    void closeCallback();
    void connectionCallback(std::shared_ptr<HttpConn> httpconn);
    bool isKeepAilve() const;
private:
    void handleError(std::string errormsg = "Bad Request", int errorcode = 400);
    void httpAnalysisRequest();
    URIState parseURI();
    HeaderState parseHeaders();
    AnalysisState analysisRequest();
    void analysisReset();
private:
    int nowReadPos_;
    ParseState hState_;
    std::weak_ptr<HttpConn> httpConn_;
    std::string inBuffer_; //inbuffer && outbuffer
    std::string outBuffer_;
    ProcessState state_;
    HttpMethod method_;
    std::map<std::string, std::string> headers_;
    bool keepAlive_;
    std::string resourceName_;
    HttpVersion HTTPVersion_;
    bool isReadAgain_; //temprarily handleError
};
