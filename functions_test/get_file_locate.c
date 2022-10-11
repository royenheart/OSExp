#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

int main(int argc, char* argv[]) {
    // 编译器内置宏，编译时插入当前源代码文件名
    printf("__FILE__: %s\n", __FILE__);
    // 编译器内置宏，编译时插入当前源代码行数
    printf("__LINE__: %d\n", __LINE__);
    printf("The source file %s is compiled in %s %s\n", __FILE__, __DATE__, __TIME__);
    printf("The path name length limits is %d\n", PATH_MAX);
    char dir[PATH_MAX] = {0};
    int ret;
    if ((ret = readlink("/proc/self/exe", dir, PATH_MAX)) > -1) {
        printf("Successfully read the link\n");
        printf("Return name length by byte: %d\n", ret);
        printf("Name is: %s\n", dir);
    } else {
        printf("Read link failed!\n");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}