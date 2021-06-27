#!/bin/bash

#!/bin/sh

# 每次构建清除上一次的cmake缓存文件
if [ -f CMakeCache.txt ]; then
    rm CMakeCache.txt
fi

cmake .
make

PWD=`pwd`
BUILD_TARTGET="build"
BUILD_VERSION="1.0.0"
BUILD_DIR="$PWD/$BUILD_TARTGET/$BUILD_VERSION"

# 创建发布目录
if [ ! -d $BUILD_DIR ]; then
    mkdir $BUILD_DIR
fi

# 将生产的业务文件，移动到发布目录，以供测试
if [ -f ./src/modules/net/myWebServerd ]; then
    cp ./src/modules/net/myWebServerd $BUILD_DIR
fi
