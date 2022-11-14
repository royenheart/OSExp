#!/bin/bash

#---------
# 性能测试
# 1. 读性能
# 2. 写性能
#---------

# 测试读性能，相同文件
function read_test_same() {
    for (( i=0; i<1000; i=i+1 )) do
        ./read tests/hello.c tests/xtfs.img > /dev/null
    done
}

# 测试写性能，不同文件
function write_test_diff() {
    for (( i=0; i<20; i=i+1 )) do
        cp tests/hello.c tests/hello${i}.c
        ./copy tests/hello${i}.c 2 tests/xtfs.img > /dev/null
        rm tests/hello${i}.c
    done
}

echo "~~~FUNCTION TEST~~~"
./format tests/xtfs.img
./copy tests/hello.c 2 tests/xtfs.img
./read tests/hello.c tests/xtfs.img
echo "~~~WRITE TEST (different files for 20 times)~~~"
time write_test_diff
echo "~~~READ TEST (same files for 1000 times)~~~"
time read_test_same