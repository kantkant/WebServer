cmake_minimum_required(VERSION 2.6)
project(myWebServer CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -std=c++11)
set(HEADERS *.h)
set(SOURCES *.cpp)
add_executable(${PROJECT_NAME}
	./WebServer/base/CountDownLatch.cpp
	./WebServer/base/CountDownLatch.h
	./WebServer/base/MutexLock.h
	./WebServer/base/CurrentThread.h
	./WebServer/base/Thread.cpp
	./WebServer/base/Thread.h
	./WebServer/base/Condition.h
	./WebServer/base/Util.cpp
	./WebServer/base/Util.h
	./WebServer/EventLoopThread.cpp
	./WebServer/EventLoopThread.h
	./WebServer/EventLoopThreadPool.cpp
	./WebServer/EventLoopThreadPool.h
	./WebServer/noncopyable.h
	./WebServer/EventLoop.cpp
	./WebServer/EventLoop.h
	./WebServer/main.cpp
	./WebServer/Epoll.cpp
	./WebServer/Epoll.h
	./WebServer/Channel.h
	./WebServer/Channel.cpp
	./WebServer/TcpServer.cpp
	./WebServer/TcpServer.h
	./WebServer/HttpConn.cpp
	./WebServer/HttpConn.h
	./WebServer/Timer.h
	./WebServer/Timer.cpp
	./WebServer/HttpServer.h
	./WebServer/HttpServer.cpp
)
target_link_libraries(${PROJECT_NAME} -lpthread)
