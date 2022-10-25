# README

- [README](#readme)
  - [编译说明](#编译说明)
    - [特性](#特性)
  - [实验流程](#实验流程)
  - [hexdump命令使用](#hexdump命令使用)

## 编译说明

### 特性

1. `Jemalloc`

    ```shell
    # Ubuntu / Debian
    apt install libjemalloc-dev
    ```

## 实验流程

1. 创建硬盘镜像文件

```shell
# 创建具有4096个数据块，每个数据块512B大小的空硬盘镜像文件
dd if=/dev/zero of=xtfs.img bs=512 count=4096
# 使用hexdump命令查看镜像内容
hexdump -C xtfs.img
```

2. copy

3. read

1. 读取inode表
2. 根据inode表读取对应文件的inode项
3. 根据inode项读取文件的数据块索引表
4. 根据数据块索引表依次读取文件的数据，并进行输出显示

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
