# README

- [README](#readme)
  - [编译说明](#编译说明)
    - [特性](#特性)
  - [实验流程](#实验流程)
    - [重要规范](#重要规范)
    - [创建硬盘镜像文件](#创建硬盘镜像文件)
    - [copy](#copy)
    - [read](#read)
    - [delete](#delete)
    - [format](#format)
    - [rename](#rename)
    - [rewrite](#rewrite)
    - [cipher & decrypt](#cipher--decrypt)
    - [HuffmanZip & HuffmanUnzip](#huffmanzip--huffmanunzip)
  - [hexdump命令使用](#hexdump命令使用)

## 编译说明

### 特性

1. `Jemalloc`

    ```shell
    # Ubuntu / Debian
    apt install libjemalloc-dev
    ```

2. `Google Test`

    ```shell
    # Ubuntu / Debian
    apt install libgtest-dev
    ```

3. `Doxygen`

    ```shell
    # Ubuntu / Debian
    apt install doxygen graphviz
    ```


## 实验流程

### 重要规范

1. 应在所有文件操作准备完全后，才能将数据写入文件系统
2. 文件类型指定必须在合法的文件类型范围内，不合法的文件类型将不被允许写入

### 创建硬盘镜像文件

```shell
# 创建具有4096个数据块，每个数据块512B大小的空硬盘镜像文件
dd if=/dev/zero of=xtfs.img bs=512 count=4096
# 使用hexdump命令查看镜像内容
hexdump -C xtfs.img
```

### copy

1. 读取inode表和数据块视图
2. 将文件中内容拷贝至xtfs文件系统
3. 将对应的数据块索引表拷贝至xtfs文件系统中
4. 更新inode表
5. 将inode表和数据块视图回写至文件系统磁盘或分区

### read

1. 读取inode表
2. 根据inode表读取对应文件的inode项
3. 根据inode项读取文件的数据块索引表
4. 根据数据块索引表依次读取文件的数据，并进行输出显示

### delete

### format

### rename

### rewrite

### cipher & decrypt

### HuffmanZip & HuffmanUnzip

## hexdump命令使用

参数：

1. `-b` 一个字节以三位8进制数输出
2. `-c` 一个字节以其代表的ASCII码显示
3. `-C` 十六进制+ASCII码显示
4. `-d` 两字节合并以十进制显示
5. `-o` 两字节合并以八进制显示
6. `-x` 两字节合并以16进制显示
7. `-e` 格式化输出
8. `-n` 指定输出多少bytes的字符长度内容
9. `-s` 指定文件读取偏移量（从0计算）
