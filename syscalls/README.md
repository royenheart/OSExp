# 系统调用探究

## 传统系统调用

需要借助软中断，用户态通过一个特殊软中断（0x80，定义在 `arch/x86/include/asm/irq_vectors.h`），进入内核执行系统调用，同时内核根据 eax 寄存器存储的值确定中断号，通过后续的 `ebx` 等寄存器获取参数。

这里涉及到如何知道被使用的系统调用的中断号：

1. 获取系统调用中断号

很多时候都推荐直接进入内核源码 `arch/x86/syscalls/syscall_32.tbl` 查看（这个文件在5.15版本内核不存在）

[不过也可以通过编译器获得](https://unix.stackexchange.com/questions/421750/where-do-you-find-the-syscall-table-for-linux)：

```bash
# 获取 sys_exit 的系统调用号，在 64 位架构上，这里打印的默认是 64 位架构的系统调用号
printf SYS_exit | gcc -include sys/syscall.h -E -
# # 0 "<stdin>"
# # 0 "<built-in>"
# # 0 "<command-line>"
# # 1 "/usr/include/stdc-predef.h" 1 3 4
# # 0 "<command-line>" 2
# # 1 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 1 3 4
# # 24 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 3 4
# # 1 "/usr/include/x86_64-linux-gnu/asm/unistd.h" 1 3 4
# # 20 "/usr/include/x86_64-linux-gnu/asm/unistd.h" 3 4
# # 1 "/usr/include/x86_64-linux-gnu/asm/unistd_64.h" 1 3 4
# # 21 "/usr/include/x86_64-linux-gnu/asm/unistd.h" 2 3 4
# # 25 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 2 3 4
# # 1 "/usr/include/x86_64-linux-gnu/bits/syscall.h" 1 3 4
# # 30 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 2 3 4
# # 0 "<command-line>" 2
# # 1 "<stdin>"
# # 1 "<stdin>" 3 4
# 60
# 想查看其他架构，比如 32 位，可以进入 header 查看代码如何根据宏来 include，比如通过 __i386__
printf SYS_exit | gcc -D __i386__ -include sys/syscall.h -E -
# # 0 "<stdin>"
# # 0 "<built-in>"
# # 0 "<command-line>"
# # 1 "/usr/include/stdc-predef.h" 1 3 4
# # 0 "<command-line>" 2
# # 1 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 1 3 4
# # 24 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 3 4
# # 1 "/usr/include/x86_64-linux-gnu/asm/unistd.h" 1 3 4
# # 16 "/usr/include/x86_64-linux-gnu/asm/unistd.h" 3 4
# # 1 "/usr/include/x86_64-linux-gnu/asm/unistd_32.h" 1 3 4
# # 17 "/usr/include/x86_64-linux-gnu/asm/unistd.h" 2 3 4
# # 25 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 2 3 4
# # 1 "/usr/include/x86_64-linux-gnu/bits/syscall.h" 1 3 4
# # 30 "/usr/include/x86_64-linux-gnu/sys/syscall.h" 2 3 4
# # 0 "<command-line>" 2
# # 1 "<stdin>"
# # 1 "<stdin>" 3 4
# 1
```

除此之外，也有一些现有的工具可以帮助我们查看，比如 `ausyscall`，在 `auditd` 软件包中：

```bash
sudo apt install auditd
ausyscall x86_64 --dump
# Using x86_64 syscall table:
# 0       read
# 1       write
# 2       open
# 3       close
ausyscall i386 --dump
# Using i386 syscall table:
# 0       restart_syscall
# 1       exit
# 2       fork
# 3       read
# 4       write
```

2. （Optional）获取系统调用具体内存地址（简单来说就是内核在内存中申请一段类似数组的地址存放各个系统调用的具体代码）

```bash
# 首先解除读取限制
echo "kernel.kptr_restrict=0" >> /etc/sysctl.conf
sysctl -p
# 通过 /proc/kallsyms 获取
cat /proc/kallsyms
# royenheart@RoyenHeartTB:~/Gits/OSExp$ cat /proc/kallsyms  | grep sys_exit
# ffffffff83102c30 T __x64_sys_exit
# ffffffff83102c50 T __ia32_sys_exit
# ffffffff83102d10 T __x64_sys_exit_group
# ffffffff83102d30 T __ia32_sys_exit_group
```

## 快速系统调用

不需要软中断，只需要进入和离开内核的指令。在 32 位系统上是 `sysenter` 和 `sysexit`，64 位系统上是 `syscall` 和 `sysret`。但不能直接使用，还需要向 MSR 寄存器等写入一些必要值指示如何正确进行。

我们可以通过 perf 比较下通过中断和快速系统调用的速度：

```bash
royenheart@RoyenHeartTB:~/Gits/OSExp/syscalls/build$ perf stat ./soft_irq_exit_i386 
 Performance counter stats for './soft_irq_exit_i386':
              0.45 msec task-clock                       #    0.413 CPUs utilized          
                 0      context-switches                 #    0.000 /sec                   
                 0      cpu-migrations                   #    0.000 /sec                   
                49      page-faults                      #  108.072 K/sec                  
         1,581,648      cycles                           #    3.488 GHz                    
           126,200      stalled-cycles-frontend          #    7.98% frontend cycles idle   
           374,923      stalled-cycles-backend           #   23.70% backend cycles idle    
           530,599      instructions                     #    0.34  insn per cycle         
                                                  #    0.71  stalled cycles per insn
           125,482      branches                         #  276.758 M/sec                  
            16,055      branch-misses                    #   12.79% of all branches        
       0.001096558 seconds time elapsed
       0.000177000 seconds user
       0.000000000 seconds sys
```

```bash
royenheart@RoyenHeartTB:~/Gits/OSExp/syscalls/build$ perf stat ./fast_syscall_64 
 Performance counter stats for './fast_syscall_64':
              0.35 msec task-clock                       #    0.410 CPUs utilized          
                 0      context-switches                 #    0.000 /sec                   
                 0      cpu-migrations                   #    0.000 /sec                   
                48      page-faults                      #  136.441 K/sec                  
         1,393,946      cycles                           #    3.962 GHz                    
           106,624      stalled-cycles-frontend          #    7.65% frontend cycles idle   
           280,083      stalled-cycles-backend           #   20.09% backend cycles idle    
           525,029      instructions                     #    0.38  insn per cycle         
                                                  #    0.53  stalled cycles per insn
           123,940      branches                         #  352.302 M/sec                  
            15,646      branch-misses                    #   12.62% of all branches        
       0.000857915 seconds time elapsed
       0.000145000 seconds user
       0.000000000 seconds sys
```

通过快速系统调用，不再需要软中断，速度更快。

glibc 封装了 syscall 命令，借助 Linux vDSO 虚拟共享库的能力，可以在用户态执行内核代码（vDSO 将内核代码映射到用户态），不需要中断，状态保存（bookkeeping），提升特权级别等一系列操作

# Reference

- https://arthurchiao.art/blog/system-call-definitive-guide-zh/
- https://blog.packagecloud.io/the-definitive-guide-to-linux-system-calls/