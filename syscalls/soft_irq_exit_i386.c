int main(int argc, char *argv[]) {
    unsigned int syscall_nr = 1;
    int exit_status = 42;

    // 由于是传统系统调用，int 0x80 在 i386 架构中有
    // sys_exit 在 i386 中是 1
    asm("movl %0, %%eax\n"
        "movl %1, %%ebx\n"
        "int $0x80"
        : /* output params, no output */
        : /* input params */
        "m"(syscall_nr), "m"(exit_status)
        : /* registers */
        "eax", "ebx");
}