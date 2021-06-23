#!/bin/bash

cmake .
make
if [[ -f CMakeCache.txt ]];then
	rm CMakeCache.txt
fi

if [[ -d CMakeFiles ]];then
	rm -rf CMakeFiles
fi

if [[ -f cmake_install.cmake ]];then
	rm  cmake_install.cmake
fi

if [[ -f Makefile ]];then
	rm Makefile
fi


