#!/bin/bash

# DEBUG
# set -x

#------------------------------
# 编译+验证
#------------------------------

mkdir -p build && cd build
cmake ..

helloC=$(mktemp tmp.XXXX)
helloMD=$(mktemp tmp.XXXX)
bigFile=$(mktemp tmp.XXXX)
MbigFile=$(mktemp tmp.XXXX)

make -j

function test_format() {
     cd $1
     ./format tests/xtfs.img

     if [[ $? == 0 ]]; then
          echo "FORMAT PASSED!"
          return 0;
     else
          echo "FORMAT FAILED!"
          return 1;
     fi
}

function test_copy_read() {
     cd $1
     ./format tests/xtfs.img && \
     ./mkdir tests tests/xtfs.img && \
     ./copy tests/hello.c 2 tests/xtfs.img && \
     ./read tests/hello.c 2 tests/xtfs.img > ${helloC} && \
     cmp ${helloC} tests/hello.c && \
     ./copy tests/hello.md 2 tests/xtfs.img && \
     ./read tests/hello.md 2 tests/xtfs.img > ${helloMD} && \
     cmp ${helloMD} tests/hello.md && \
     ./copy tests/long_hello.md 2 tests/xtfs.img && \
     ./read tests/long_hello.md 2 tests/xtfs.img > ${bigFile} && \
     cmp ${bigFile} tests/long_hello.md && \
     ./copy tests/bigFile.md 2 tests/xtfs.img && \
     ./read tests/bigFile.md 2 tests/xtfs.img > ${MbigFile} && \
     cmp ${MbigFile} tests/bigFile.md

     if [[ $? == 0 ]]; then
          echo "COPY READ PASSED!"
          return 0;
     else
          echo "COPY READ FAILED!"
          return 1;
     fi
}

function test_rewrite_read() {
     cd $1
     ./format tests/xtfs.img && \
     ./mkdir tests/ tests/xtfs.img && \
     ./copy tests/hello.c 2 tests/xtfs.img && \
     ./rewrite tests/hello.c 2 tests/hello.md tests/xtfs.img && \
     ./read tests/hello.c 2 tests/xtfs.img > ${helloMD} && \
     cmp ${helloMD} tests/hello.md && \
     ./rewrite tests/hello.c 2 tests/hello.c tests/xtfs.img && \
     ./read tests/hello.c 2 tests/xtfs.img > ${helloC} && \
     cmp ${helloC} tests/hello.c && \
     ./rewrite tests/hello.c 2 tests/long_hello.md tests/xtfs.img && \
     ./read tests/hello.c 2 tests/xtfs.img > ${bigFile} && \
     cmp ${bigFile} tests/long_hello.md && \
     ./rewrite tests/hello.c 2 tests/hello.md tests/xtfs.img && \
     ./read tests/hello.c 2 tests/xtfs.img > ${helloMD} && \
     cmp ${helloMD} tests/hello.md && \
     ./rewrite tests/hello.c 2 tests/bigFile.md tests/xtfs.img && \
     ./read tests/hello.c 2 tests/xtfs.img > ${MbigFile} && \
     cmp ${MbigFile} tests/bigFile.md && \
     ./rewrite tests/hello.c 2 tests/hello.md tests/xtfs.img && \
     ./read tests/hello.c 2 tests/xtfs.img > ${helloMD} && \
     cmp ${helloMD} tests/hello.md

     if [[ $? == 0 ]]; then
          echo "REWRITE READ PASSED!"
          return 0;
     else
          echo "REWRITE READ FAILED!"
          return 1;
     fi
}

function test_mv_delete_read() {
     cd $1
     ./format tests/xtfs.img && \
     ./mkdir tests/ tests/xtfs.img && \
     ./mkdir ano tests/xtfs.img && \
     ./copy tests/hello.c 2 tests/xtfs.img && \
     ./mv tests/hello.c 2 / tests/xtfs.img && \
     ./read hello.c 2 tests/xtfs.img > ${helloC} && \
     cmp ${helloC} tests/hello.c && \
     ./copy tests/bigFile.md 2 tests/xtfs.img && \
     ./mv tests/bigFile.md 2 ano tests/xtfs.img && \
     ./read ano/bigFile.md 2 tests/xtfs.img > ${MbigFile} && \
     cmp ${MbigFile} tests/bigFile.md && \
     ./mv ano/ 3 tests tests/xtfs.img && \
     ./read tests/ano/bigFile.md 2 tests/xtfs.img > ${MbigFile} && \
     cmp ${MbigFile} tests/bigFile.md && \
     ./delete tests/ano/bigFile.md 2 tests/xtfs.img

     if [[ $? == 0 ]]; then
          echo "MV DELETE READ PASSED!"
          return 0;
     else
          echo "MV DELETE READ FAILED!"
          return 1;
     fi
}

function test_cipher_read() {
     cd $1
     ./format tests/xtfs.img && \
     ./mkdir tests tests/xtfs.img && \
     ./cipher tests/hello.c 2 tests/xtfs.img 123456 && \
     ./decrypt tests/hello.c 2 tests/xtfs.img 123456 > ${helloC} && \
     cmp ${helloC} tests/hello.c && \
     ./cipher tests/hello.md 2 tests/xtfs.img 123456 && \
     ./decrypt tests/hello.md 2 tests/xtfs.img 123456 > ${helloMD} && \
     cmp ${helloMD} tests/hello.md && \
     ./cipher tests/long_hello.md 2 tests/xtfs.img 123456 && \
     ./decrypt tests/long_hello.md 2 tests/xtfs.img 123456 > ${bigFile} && \
     cmp ${bigFile} tests/long_hello.md && \
     ./cipher tests/bigFile.md 2 tests/xtfs.img 123456 && \
     ./decrypt tests/bigFile.md 2 tests/xtfs.img 123456 > ${MbigFile} && \
     cmp ${MbigFile} tests/bigFile.md

     if [[ $? == 0 ]]; then
          echo "CIPHER READ PASSED!"
          return 0;
     else
          echo "CIPHER READ FAILED!"
          return 1;
     fi
}

function test_zip_read() {
     cd $1
     ./format tests/xtfs.img && \
     ./mkdir tests tests/xtfs.img && \
     ./HuffmanZip tests/hello.c 2 tests/xtfs.img && \
     ./HuffmanUnzip tests/hello.c 2 tests/xtfs.img > ${helloC} && \
     cmp ${helloC} tests/hello.c && \
     ./HuffmanZip tests/hello.md 2 tests/xtfs.img && \
     ./HuffmanUnzip tests/hello.md 2 tests/xtfs.img > ${helloMD} && \
     cmp ${helloMD} tests/hello.md && \
     ./HuffmanZip tests/long_hello.md 2 tests/xtfs.img && \
     ./HuffmanUnzip tests/long_hello.md 2 tests/xtfs.img > ${bigFile} && \
     cmp ${bigFile} tests/long_hello.md && \
     ./HuffmanZip tests/bigFile.md 2 tests/xtfs.img && \
     ./HuffmanUnzip tests/bigFile.md 2 tests/xtfs.img > ${MbigFile} && \
     cmp ${MbigFile} tests/bigFile.md 

     if [[ $? == 0 ]]; then
          echo "ZIP READ PASSED!"
          return 0;
     else
          echo "ZIP READ FAILED!"
          return 1;
     fi
}

function gtest_test() {
     cd $1
     ctest

     if [[ $? == 0 ]]; then
          echo "GTEST FUNCS PASSED!"
          return 0;
     else
          echo "GTEST FUNCS FAILED!"
          return 1;
     fi
}

test_format $(pwd) && \
test_copy_read $(pwd) && \
test_rewrite_read $(pwd) && \
test_mv_delete_read $(pwd) && \
test_cipher_read $(pwd) && \
test_zip_read $(pwd) && \
gtest_test $(pwd)

if [[ $? == 0 ]]; then
     echo "TEST BUILD PASSED!"
else
     echo "TEST BUILD FAILED!"
fi

rm ${helloC}
rm ${helloMD}
rm ${bigFile}
rm ${MbigFile}