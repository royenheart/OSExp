#!/bin/bash

#------------------------------
# 编译+验证
#------------------------------

mkdir -p build && cd build
cmake ..
make && ./format tests/xtfs.img \
     && ./copy tests/hello.c 2 tests/xtfs.img \
     && ./read tests/hello.c tests/xtfs.img \
     && ./rename tests/hello.c tests/hello.cpp tests/xtfs.img \
     && ./read tests/hello.cpp tests/xtfs.img \
     && ./delete tests/hello.cpp tests/xtfs.img \
     && ./copy tests/hello.cpp 2 tests/xtfs.img \
     && ./read tests/hello.cpp tests/xtfs.img \
     && ./rewrite tests/hello.cpp tests/hello.c tests/xtfs.img \
     && ./read tests/hello.c tests/xtfs.img \
     && ./rewrite tests/hello.c tests/hello.md tests/xtfs.img \
     && ./read tests/hello.md tests/xtfs.img \
     && ./delete tests/hello.md tests/xtfs.img \
     && ./cipher tests/hello.md 2 tests/xtfs.img 123456 \
     && ./decrypt tests/hello.md tests/xtfs.img 123456\
     && ./cipher tests/hello.cpp 2 tests/xtfs.img 123456 \
     && ./decrypt tests/hello.cpp tests/xtfs.img 123456 \
     && ./delete tests/hello.md tests/xtfs.img \
     && ./delete tests/hello.cpp tests/xtfs.img \
     && ./HuffmanZip tests/hello.md 2 tests/xtfs.img \
     && ./HuffmanUnzip tests/hello.md tests/xtfs.img \
     && ./HuffmanZip tests/hello.cpp 2 tests/xtfs.img \
     && ./HuffmanUnzip tests/hello.cpp tests/xtfs.img \

if [[ $? == 0 ]]; then
     echo "TEST BUILD PASSED!"
else
     echo "TEST BUILD FAILED!"
fi