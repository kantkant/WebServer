# Install script for directory: /home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/zxx/Desktop/WebServer/demo/build/release-install-cpp11")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/zxx/Desktop/WebServer/demo/build/release-cpp11/lib/libmuduo_base.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/muduo/base" TYPE FILE FILES
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/AsyncLogging.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Atomic.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/BlockingQueue.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/BoundedBlockingQueue.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Condition.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/CountDownLatch.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/CurrentThread.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Date.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Exception.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/FileUtil.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/GzipFile.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/LogFile.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/LogStream.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Logging.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Mutex.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/ProcessInfo.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Singleton.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/StringPiece.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Thread.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/ThreadLocal.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/ThreadLocalSingleton.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/ThreadPool.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/TimeZone.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Timestamp.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/Types.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/WeakCallback.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/copyable.h"
    "/home/zxx/Desktop/WebServer/demo/muduo-cpp11/muduo/base/noncopyable.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/zxx/Desktop/WebServer/demo/build/release-cpp11/muduo/base/tests/cmake_install.cmake")

endif()

