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
)
target_link_libraries(${PROJECT_NAME} -lpthread)
