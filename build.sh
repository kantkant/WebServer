#!/bin/bash

#!/bin/sh
set -x
# 每次构建清除上一次的cmake缓存文件
if [ -f CMakeCache.txt ]; then
    rm CMakeCache.txt
fi

if [ myWebServerd ]; then
    rm myWebServerd
fi

cmake .
make

PWD=`pwd`
BUILD_DIR="$PWD"

# 创建发布目录
if [ ! -d $BUILD_DIR ]; then
    mkdir $BUILD_DIR
fi

# 将生产的业务文件，移动到发布目录，以供测试
if [ -f ./src/myWebServerd ]; then
    cp ./src/myWebServerd $BUILD_DIR
fi
