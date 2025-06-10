#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define READ_SIZE 1024
#define PORT 8080

int server_fd, epoll_fd;

void release() {
    printf("Release\n");
    close(server_fd);
    close(epoll_fd);
}

// 设置文件描述符为非阻塞模式
// 使用 fcntl() 将服务器和客户端的套接字设置为非阻塞模式，这样在使用 epoll
// 的边缘触发模式时，避免出现阻塞问题。
// 阻塞模式：操作会等待直到完成，适合简单的程序，但可能导致程序挂起。
// 非阻塞模式：操作无法立即完成时立即返回错误，错误代码设置为 `EAGAIN` 或
// `EWOULDBLOCK` 等，表示资源 I/O
// 操作资源当前暂时不可用，适合高并发应用，但编程复杂度较高。需要程序判断是需要稍后再试还是先处理其他任务等，状态复杂。
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }
    return 0;
}

// 处理客户端连接
void handle_connection(int client_fd) {
    char buffer[READ_SIZE];
    ssize_t bytes_read = read(client_fd, buffer, READ_SIZE - 1);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Received: %s\n", buffer);
        write(client_fd, buffer, bytes_read);  // 回显消息
    } else if (bytes_read == 0) {
        // 客户端断开连接
        printf("Client disconnected\n");
        close(client_fd);
    } else {
        perror("read");
    }
}

int main(int argc, char *argv[]) {
    int client_fd;
    // Internet Socket Address Description
    struct sockaddr_in server_addr;
    struct epoll_event ev, events[MAX_EVENTS];
    int nfds, i;
    int opt = 1;

    signal(SIGINT, release);

    // 创建服务器 socket
    // Address Families 地址族（domain），用于制定 socket
    // 地址类型。不同地址族支持不同协议和网络类型：
    // 1. AF_INET：表示 ipv4 协议的地址族
    // 2. AF_INET6：表示 ipv6 协议的地址族
    // Sockets 类型（socket_type.h）：
    // 1. SOCK_STREAM：面向连接的，可靠的字节流服务，通常为 TCP 协议。Sequenced,
    // reliable, connection-based byte streams.
    // 2. SOCK_DGRAM：无连接的，数据报格式的通信。Connectionless, unreliable
    // datagrams of fixed maximum length.
    // 3. SOCK_RAW：原始协议接口，可用于自定义协议或进行网络分析。
    // 4. SOCK_SEQPACKET：类似
    // SOCK_STREAM，但是消息是固定大小的报文通信，通常用于面向连接的协议如
    // SCTP。Sequenced, reliable, connection-based, datagrams of fixed maximum
    // length. socket：创建 socket 并返回文件描述符。(int __domain, int __type,
    // int __protocal)，指定地址类型、Socket 类型、使用的协议（0 表示自动选择）
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket can't create");
        exit(EXIT_FAILURE);
    }

    // 设置 SO_REUSEPORT 选项
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) <
        0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // host to network byte order
    server_addr.sin_port = htons(PORT);

    // 绑定端口
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
        -1) {
        perror("socket can't bind to addr");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 监听端口
    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("socket can't bind to port");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 创建 epoll 实例
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 将 server_fd 加入 epoll 实例中
    // epoll_ctl：用于向 epoll 实例添加、删除、修改 fd 上事件 event 的监听
    // ev：用于描述被添加到 epoll 实例的文件描述符的事件
    // 属性 events 表示事件类型
    // 1. EPOLLIN，表示 fd 上有数据可读事件（EPOLL IN）
    // 2. EPOLLOUT，表示 fd 上写数据事件，用于监控输出事件等。
    // 2. EPOLLET，设置为边缘触发方式（默认水平触发，level-triggered）
    // 属性 data 可存储一些与事件相关的数据
    // typedef union epoll_data
    // {
    //   void *ptr;
    //   int fd;
    //   uint32_t u32;
    //   uint64_t u64;
    // } epoll_data_t;
    // fd 将事件与 fd 进行联系，方便后续的判断
    // EPOLL 实例操作码:
    // 1. EPOLL_CTL_ADD
    // 2. EPOLL_CTL_DEL
    // 3. EPOLL_CTL_MOD，修改已添加的
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl: server_fd");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // 事件循环
    while (1) {
        // timeout 为 -1，epoll 阻塞直到一个事件变为就绪，传入 events。
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            close(server_fd);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < nfds; ++i) {
            // 当接受到的事件发生在开启的服务端 Socket 上时
            if (events[i].data.fd == server_fd) {
                // 处理新的客户端连接
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                                   &client_addr_len);
                if (client_fd == -1) {
                    perror("accept");
                    continue;
                }

                // 设置客户端 socket 为非阻塞模式
                if (set_nonblocking(client_fd) == -1) {
                    close(client_fd);
                    continue;
                }

                // 将新的客户端 socket 加入 epoll 实例
                // 接收到客户端发送数据，并只读取一次
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                    perror("epoll_ctl: client_fd");
                    close(client_fd);
                    continue;
                }

                printf("Accepted connection from client\n");
            } else {
                // 处理客户端请求
                handle_connection(events[i].data.fd);
            }
        }
    }

    release();
    return 0;
}
