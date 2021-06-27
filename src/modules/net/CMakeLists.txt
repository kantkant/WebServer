cmake_minimum_required(VERSION 2.6)
project(myWebServerd CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS -std=c++11)
set(HEADERS *.h)
set(SOURCES *.cpp)

FILE(GLOB HEADERS *.h)
FILE(GLOB SOURCES *.cpp)
FILE(GLOB MODULE_SOURCES ../base/*.cpp)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${MODULE_SOURCES})
target_link_libraries(${PROJECT_NAME} -lpthread)

# TODO：将业务代码抽离，后续功能类代码，所有的测试文件单独在test里面
# TODO：将应用层代码抽离到app目录下面
install(FILES ${project} DESTINATION ../../../build/1.0.0)