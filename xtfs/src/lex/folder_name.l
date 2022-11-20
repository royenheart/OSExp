%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../xtfs_limits.h"
#include "../xtfs_manage.h"
#include "folder_lex.h"
// 文件夹
#define T_FOLDER 1
// 无文件夹指定错误
#define T_NONE 2
// 文件夹名称格式错误
#define T_ERROR 3
// 文件夹名字过长
#define T_NAME_TOO_LONG 4
// 文件夹深度过大
#define T_ITER_TOO_DEEP 5
// 读到匹配串尾部
#define T_END 0
int delim = 0;
int folder_num = -1;
const int max_iter_folder = MAX_ITER_FOLDER;
%}

FOLDER ([a-zA-Z0-9\-_\.]+|\"{1}[a-zA-Z0-9\.\-_ ]+\"{1}|\"{1}[a-zA-Z0-9\.\-_ ]+\"{1})

%%
{FOLDER} {folder_num++; return (folder_num > max_iter_folder - 1)?T_ITER_TOO_DEEP:T_FOLDER;}
<<EOF>> {return (folder_num < 0)?T_NONE:T_END;}
\/ {delim++;}
. {return T_ERROR;}
%%

// int main(int argc, char* argv[]) {
int get_folders(const char* strs, char ***store) {
    int token_type;
    FILE *fp;
    int i;
    // char **matches[MAX_ITER_FOLDER + 1][MAX_FILE_NAME_LENGTH] = {0};
    // char **matches = (char**)malloc((MAX_ITER_FOLDER + 1) * sizeof(char**));
    *store = (char**)malloc((MAX_ITER_FOLDER + 1) * sizeof(char**));
    char tmpF[] = "/tmp/temp_file.XXXXXX";
    int fd = mkstemp(tmpF);
    // 写方式打开临时文件
    fp = fopen(tmpF, "w");
    // 输入重定向为临时文件
    freopen(tmpF, "r", stdin);
    // 向临时文件写
    fwrite(strs, 1, strlen(strs), fp);
    // 写完后关闭
    fclose(fp);

    for (i = 0; i < MAX_ITER_FOLDER + 1; i++) {
        // matches[i] = (char*)malloc(MAX_FILE_NAME_LENGTH * sizeof(char*));
        // memset(matches[i], 0, MAX_FILE_NAME_LENGTH * sizeof(char));
        (*store)[i] = (char*)malloc(MAX_FILE_NAME_LENGTH * sizeof(char*));
        memset((*store)[i], 0, MAX_FILE_NAME_LENGTH * sizeof(char));
    }

    while ((token_type = yylex()) == T_FOLDER) {
        if (strlen(yytext) > MAX_FILE_NAME_LENGTH) {
            token_type = T_NAME_TOO_LONG;
            break;
        }
        // strcpy(matches[folder_num], yytext);
        strcpy((*store)[folder_num], yytext);
        // printf("FOLDER: %s,%d\n", yytext, strlen(yytext));
        // printf("FOLDER_MATCHES: %s,%d\n", matches[folder_num], strlen(matches[folder_num]));
    }

    // 删除文件的目录入口，由于之前的文件被用完整文件名打开，故要结束后在这里取消
    unlink(tmpF);
    // 通过文件描述符关闭文件，由于之前使用文件名打开的输出流已关闭，且删除了文件的目录入口，因此关闭后文件将被自动删除。
    close(fd);

    int ret = folder_num;
    // 生成到源代码是全局变量，多次读取会导致位置问题
    delim = 0;
    folder_num = -1;

    if (token_type == T_ERROR || token_type == T_NONE || 
        token_type == T_NAME_TOO_LONG || token_type == T_ITER_TOO_DEEP) {
        if (token_type == T_ERROR) {
            printf("Not a regular folder!\n");
        } else if (token_type == T_NONE) {
            printf("No folder specified!\n");
        } else if (token_type == T_NAME_TOO_LONG) {
            printf("Folder name %s too long!\n", yytext);
        } else if (token_type == T_ITER_TOO_DEEP) {
            printf("Folder iter too deep!\n");
        }
        // free(matches);
        // 后面使用统一的内存管理机制（xtfs_manage.c）进行管理。
        // free(*store);
        return -1;
    }

    return ret;
}

int yywrap() {
    return 1;
}