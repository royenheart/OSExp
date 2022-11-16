# TODO list

## BUG

- [ ] 由于没有加入目录这种东西，导致文件名的保存会有问题
- [ ] 存在可能的缓冲区溢出问题
- [ ] 应该在数据和数据块索引表都有空闲块可以使用时，再真正写入文件系统，目前并没有做这方面的检查

## 基础库（C）

- [ ] Testbenches
- [ ] 添加针对Linux信号的对应操作（比如遇到ctrl+c时应如何处理当前的事务）
- [x] 添加删除、重写（相同文件改变）、剪切、创建功能
- [ ] 二级数据块索引
- [ ] 统一的内存管理、退出机制、错误信息管理
- [ ] 加入目录的支持
- [ ] 计算限制

    ```c
    short* get_all_block(int need) {
        short *blocknr_s = (short*)malloc(BLOCK_SIZE * sizeof(short));
        memset(blocknr_s, 0, BLOCK_SIZE * sizeof(short));
        int i;
        for (i = 0; i < need; i++) {
            blocknr_s[i] = get_block();
        }
	    return blocknr_s;
    }
    ```

    这里的BLOCK_SIZE * sizeof(short)的数据块索引大小等还需要进行重新计算，看怎么分配适合。并能迭代到二级数据块索引。

## 进阶/终端设计（C++）

- [ ] 使用库进行参数读入、输入输出、日志管理、资源回收等
- [ ] Unicode多语言读入支持
- [x] doxgen支持
