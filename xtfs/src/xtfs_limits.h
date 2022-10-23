#pragma once

// 数据块大小
#define BLOCK_SIZE 512
// inode数据结构大小
#define INODE_SIZE sizeof(struct inode)
// inode表大小
#define NR_INODE (BLOCK_SIZE / INODE_SIZE)
// 最大文件名长度
#define MAX_FILE_NAME_LENGTH 20
// 最大文件系统文件名长度
#define MAX_FS_NAME_LENGTH 20