cmake_minimum_required(VERSION 2.6)
project(myWebServerd CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -std=c++11)

# 添加gdb调试
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(HEADERS *.h)
set(SOURCES *.cpp)

FILE(GLOB HEADERS *.h)
FILE(GLOB SOURCES *.cpp)
FILE(GLOB MODULE_SOURCES ./base/*.cpp)
FILE(GLOB LOG_SOURCES ./log/*.cpp)

add_definitions(-DBOOST_LOG_DYN_LINK)
add_definitions(-BOOST_ALL_DYN_LINK)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${MODULE_SOURCES} ${LOG_SOURCES})
target_link_libraries(${PROJECT_NAME}
            ${Boost_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${GLOG_LIBRARIES}
            boost_log_setup
            boost_log
            boost_thread
            boost_system
            boost_date_time
            pthread)


add_subdirectory(base) 
# TODO：将业务代码抽离，后续功能类代码，所有的测试文件单独在test里面
# TODO：将应用层代码抽离到app目录下面
install(FILES ${project} DESTINATION ../../../)
