#!/bin/bash

#------------------------------
# 编译+验证
#------------------------------

mkdir -p build && cd build
cmake ..
make && ./format test_file/xtfs.img \
     && ./copy test_file/hello.c 2 test_file/xtfs.img \
     && ./read test_file/hello.c test_file/xtfs.img \
     && ./rename test_file/hello.c test_file/hello.cpp test_file/xtfs.img \
     && ./read test_file/hello.cpp test_file/xtfs.img

if [[ $? == 0 ]]; then
     echo "TEST BUILD PASSED!"
else
     echo "TEST BUILD FAILED!"
fi