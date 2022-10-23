#!/bin/bash

#------------------------------
# 编译+验证
# 后面加上Google Test进行自动测试
#------------------------------

mkdir -p build && cd build
cmake ..
make && ./format test_file/xtfs.img \
     && ./copy test_file/hello.c 2 test_file/xtfs.img \
     && ./read test_file/hello.c test_file/xtfs.img