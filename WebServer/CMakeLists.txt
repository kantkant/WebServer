cmake_minimum_required(VERSION 2.6)
project(WebServer CXX)
set(CMAKE_CXX_STANDARD 11)
set(HEADERS *.h)
set(SOURCES *.cpp)
add_executable(${PROJECT_NAME}
	CountDownLatch.cpp
	EventLoopThread.cpp
	CountDownLatch.h
	EventLoopThread.h
	MutexLock.h
	CurrentThread.h
	EventLoopThreadPool.cpp
	noncopyable.h
	EventLoop.cpp
	EventLoopThreadPool.h
	Thread.cpp
	Condition.h
	EventLoop.h
	main.cpp
	Thread.h
	Epoll.cpp
	Epoll.h
	Util.cpp
	Util.h
	Channel.h
	Channel.cpp
	TcpServer.cpp
	TcpServer.h
	HttpConn.cpp
	HttpConn.h
	Timer.h
	Timer.cpp
	HttpServer.h
	HttpServer.cpp
)
target_link_libraries(${PROJECT_NAME} -lpthread)
