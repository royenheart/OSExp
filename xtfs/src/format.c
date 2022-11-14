/**
 * @file format.c
 * @author RoyenHeart, Ruoxuan Wang, MDND, bow
 * @brief 格式化镜像、分区文件系统
 * @version 0.1
 * @date 2022-10-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xtfs_limits.h"
#include "xtfs_check.h"

/**
 * @brief 格式化
 * 
 * @param argc 参数个数
 * @param argv 参数内容
 * @return int 0 成功格式化
 */
int main(int argc, char* argv[])
{
    FILE *fp = NULL;
    char *fs_name = NULL;
    char blank[BLOCK_SIZE] = {0};
    
    check_fs_name(argv[1]);

    fs_name = argv[1];
    // 以可读可写模式打开xtfs.img文件
    fp = fopen(fs_name, "r+");
    fwrite(blank, 1, BLOCK_SIZE, fp);
    fwrite(blank, 1, BLOCK_SIZE, fp);
    fseek(fp, 512, SEEK_SET);
    fputc(3, fp);
    fclose(fp);

    return(EXIT_SUCCESS);
}

