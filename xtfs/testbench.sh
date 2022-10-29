#!/bin/bash

#---------
# 性能测试
# 1. 读性能
# 2. 写性能
#---------

function read_test() {
    for (( i=0; i<100; i=i+1 )) do
        ./read test_file/hello.c test_file/xtfs.img
    done
}

./format test_file/xtfs.img
./copy test_file/hello.c 2 test_file/xtfs.img
time read_test