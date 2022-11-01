/**
 * @file format.c
 * @author
 * @brief 格式化镜像、分区文件系统
 * @version 0.1
 * @date 2022-10-25
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 格式化
 *
 * @param argc 参数个数
 * @param argv 参数内容
 * @return int 0 成功格式化
 */
int main(int argc, char* argv[]) {
    FILE* fp = NULL;
    char* fs_name = NULL;

    fs_name = argv[1];
    // 以可读可写模式打开xtfs.img文件
    fp = fopen(fs_name, "r+");
    // 更改被写入文件的指针，使其写入正确的扇区内
    fseek(fp, 512, SEEK_SET);
    // 将指定数据写入第一块扇区
    fputc(3, fp);
    fclose(fp);

    return (EXIT_SUCCESS);
}

