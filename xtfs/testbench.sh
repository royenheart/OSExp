#!/bin/bash

#---------
# 性能测试
# 1. 读性能
# 2. 写性能
#---------

# 测试读性能，相同文件
function read_test_same() {
    for (( i=0; i<1000; i=i+1 )) do
        ./read test_file/hello.c test_file/xtfs.img > /dev/null
    done
}

# 测试写性能，不同文件
function write_test_diff() {
    for (( i=0; i<20; i=i+1 )) do
        cp test_file/hello.c test_file/hello${i}.c
        ./copy test_file/hello${i}.c 2 test_file/xtfs.img > /dev/null
        rm test_file/hello${i}.c
    done
}

echo "~~~FUNCTION TEST~~~"
./format test_file/xtfs.img
./copy test_file/hello.c 2 test_file/xtfs.img
./read test_file/hello.c test_file/xtfs.img
echo "~~~WRITE TEST (different files for 20 times)~~~"
time write_test_diff
echo "~~~READ TEST (same files for 1000 times)~~~"
time read_test_same