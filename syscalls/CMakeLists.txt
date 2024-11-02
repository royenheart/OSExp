cmake_minimum_required(VERSION 3.16)

project(syscalls)

add_executable(soft_irq_exit_i386 soft_irq_exit_i386.c)
add_executable(fast_syscall_64 fast_syscall_64.c)
add_executable(glibc_syscall glibc_syscall.c)