#!/bin/bash

#---------
# 性能测试
# 1. COPY 复制文件性能(小/大文件)
# 2. READ 读文件性能(小/大文件)
# 3. Huffman解压缩 文件性能(小/大文件)
# 4. 加密解密 文件性能(小/大文件)
#---------

# 测试 READ 性能
function read_test() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    ./copy tests/hello.c 2 tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./read tests/hello.c 2 tests/xtfs.img > /dev/null
    done
}

# 测试 READ 大文件性能
function read_test_bigio() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    ./copy tests/bigFile.md 2 tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./read tests/bigFile.md 2 tests/xtfs.img > /dev/null
    done
}

# 测试 COPY 性能
function copy_test() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./copy tests/hello.c 2 tests/xtfs.img
        ./delete tests/hello.c 2 tests/xtfs.img
    done
}

# 测试 COPY 大文件性能
function copy_test_bigio() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./copy tests/bigFile.md 2 tests/xtfs.img > /dev/null
        ./delete tests/bigFile.md 2 tests/xtfs.img
    done
}

# 测试 Huffman解压缩 性能
function huffmanZip_test() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./HuffmanZip tests/hello.c 2 tests/xtfs.img
        ./HuffmanUnzip tests/hello.c 2 tests/xtfs.img > /dev/null
        ./delete tests/hello.c 34 tests/xtfs.img
    done
}

# 测试 Huffman解压缩 大文件性能
function huffmanZip_test_bigio() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./HuffmanZip tests/bigFile.md 2 tests/xtfs.img
        ./HuffmanUnzip tests/bigFile.md 2 tests/xtfs.img > /dev/null
        ./delete tests/bigFile.md 34 tests/xtfs.img
    done
}

# 测试 加密解密 性能
function crypt_test() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./cipher tests/hello.c 2 tests/xtfs.img 123456
        ./decrypt tests/hello.c 2 tests/xtfs.img 123456 > /dev/null
        ./delete tests/hello.c 66 tests/xtfs.img
    done
}

# 测试 加密解密 大文件性能
function crypt_test_bigio() {
    ./format tests/xtfs.img
    ./mkdir tests tests/xtfs.img
    for (( i=0; i<1000; i=i+1 )) do
        ./cipher tests/bigFile.md 2 tests/xtfs.img 123456
        ./decrypt tests/bigFile.md 2 tests/xtfs.img 123456 > /dev/null
        ./delete tests/bigFile.md 66 tests/xtfs.img
    done
}

echo "~~~FUNCTION TEST~~~"
./test.sh
cd build
echo "~~~COPY TEST(same file for 1000 times)~~~"
echo "Small File:"
time copy_test
echo "Big File:"
time copy_test_bigio
echo "~~~READ TEST(same file for 1000 times)~~~"
echo "Small File:"
time read_test
echo "Big File:"
time read_test_bigio
echo "~~~Huffman Zip/Unzip TEST(same file for 1000 times)~~~"
echo "Small File:"
time huffmanZip_test
echo "Big File:"
time huffmanZip_test_bigio
echo "~~~Cipher/Decrypt TEST(same file for 1000 times)~~~"
echo "Small File:"
time crypt_test
echo "Big File:"
time crypt_test_bigio