# TODO list

## BUG

- [x] 由于没有加入目录这种东西，导致文件名的保存会有问题
- [x] 存在可能的缓冲区溢出问题
- [x] 应该在数据和数据块索引表都有空闲块可以使用时，再真正写入文件系统，目前并没有做这方面的检查

## 基础库（C）

- [x] Testbenches
- [ ] 添加针对Linux信号的对应操作（比如遇到ctrl+c时应如何处理当前的事务）
- [x] 添加删除、重写（相同文件改变）、剪切、创建功能
- [x] 二级数据块索引
- [ ] 添加重命名功能（可选）
- [ ] 统一的内存管理、退出机制、错误信息管理
- [x] 加入目录的支持
- [ ] 支持多种特定格式文件同时处理，比如支持同时加密和解密
- [ ] 理清文件系统文件传输方式，整合各个方法
- [ ] 合理化文件类型判断，比如 read 是否需要根据文件类型进行读写
- [ ] 类型参数更好的输入,例如可以输入 2+CIPHER 参数转换成对应的类型
- [ ] 优化 HuffmanZip 对大文件的读写
- [ ] 目录支持多级数据块索引（即目录下支持更多的文件）
- [x] 计算限制

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

    2022/11/18，修正为：

    ```c
    short* get_all_block(int need, BLOCK_MAP_STRUC* block_map, BLOCK_MAP_TABLE_STRUC* lowbit) {
        // 必须返回 need + 1 空间，防止缓冲区溢出，但是申请的时候还是按照 need 数量来
        if (need <= 0) {
            return memset((INDEX_TABLE_STRUC*)xtfs_malloc(sizeof(INDEX_TABLE_STRUC)), 0, sizeof(INDEX_TABLE_STRUC));
        }
        INDEX_TABLE_STRUC* blocknr_s = (INDEX_TABLE_STRUC*)xtfs_malloc((need + 1) * sizeof(INDEX_TABLE_STRUC));
        memset(blocknr_s, 0, (need + 1) * sizeof(INDEX_TABLE_STRUC));
        int i;
        for (i = 0; i < need; i++) {
            blocknr_s[i] = get_block(block_map, lowbit);
        }
        return blocknr_s;
    }
    ```

## 进阶/终端设计（C++）

- [ ] 使用库进行参数读入、输入输出、日志管理、资源回收等
- [ ] Unicode多语言文件内容读取、文件和目录名称支持
- [x] doxgen支持
