#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 以全零创建大小为2M的xtfs.img文件
 * 
 */

// 文件系统文件名
char *fs_name;

/**
 * @brief 格式化
 * 
 * @param argc 参数个数
 * @param argv 参数内容
 * @return int 0 成功格式化
 */
int main(int argc, char* argv[])
{
    FILE *fp;

    fs_name = argv[1];
    fp = fopen(fs_name, "r+");       //以可读可写模式打开xtfs.img文件
    fseek(fp, 512, SEEK_SET);           //更改被写入文件的指针，使其写入正确的扇区内
    fputc(3, fp);                       //将指定数据写入第一块扇区
    fclose(fp);

    return(EXIT_SUCCESS);
}

