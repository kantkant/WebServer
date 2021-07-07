cmake_minimum_required(VERSION 2.8)

project(WebServer CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CXX_FLAGS
    -g
    -Wall
    -std=c++11
    -D_PTHREADS
    -Wno-unused-parameter
)


set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
add_definitions(-DBOOST_LOG_DYN_LINK)
add_definitions(-BOOST_ALL_DYN_LINK)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

add_executable(logtest logtest.cpp ../log.h ../log.cpp)
target_link_libraries(logtest
            ${Boost_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${GLOG_LIBRARIES}
            boost_log_setup
            boost_log
            boost_thread
            boost_system
            boost_date_time
            pthread
            ) 