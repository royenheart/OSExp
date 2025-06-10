#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#define SERVER_PORT 8080
#define MAX_EVENTS 10
#define READ_SIZE 1024

int sockfd, epoll_fd;

void release() {
    printf("Release\n");
    close(sockfd);
    close(epoll_fd);
}

// 设置文件描述符为非阻塞模式
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

// 非阻塞 + epoll 的处理顺序（时序），每次运行 epoll
// 事件队列内的排布可能会有所不同，需要仔细设置状态。例如 client
// 同样执行三次出现三种不同的结果：
// 1. client epoll 发现写缓冲好了触发一次，但此时 server 可能还没触发或没执行到
// recv / read 操作，导致两次发送的数据都放在缓冲区中。
// royenheart@RoyenHeartFedora:~/gits/OSExp/build/epoll_test$ ./epoll_client
// Sent message to server: Hello, server!
// Sent message to server in loop 2: Another!
// Received from server in loop 3: Hello, server!Another!
// ^C
// 2. 正常顺序，发送后 server 接收，返回，client
// 被触发，同时也发现写缓冲区就位。
// royenheart@RoyenHeartFedora:~/gits/OSExp/build/epoll_test$ ./epoll_client
// Sent message to server: Hello, server!
// Received from server in loop 2: Hello, server!
// Sent message to server in loop 2: Another!
// Received from server in loop 3: Another!
// ^C
// 3. 到第二次发送时（发现写缓冲好了），server
// 已经接受好了第一次的缓冲，但是还没到返回
// write。第二次发送好了返回，于是接连传输了两次（触发了两次 client
// epoll），然后写缓冲准备就绪事件放入队列，在 loop4 被发送，loop5 回显。
// royenheart@RoyenHeartFedora:~/gits/OSExp/build/epoll_test$ ./epoll_client
// Sent message to server: Hello, server!
// Sent message to server in loop 2: Another!
// Received from server in loop 3: Hello, server!
// Received from server in loop 4: Another!
// Sent message to server in loop 4: Another!
// Received from server in loop 5: Another!
// ^C
int main(int argc, char *argv[]) {
    struct sockaddr_in server_addr;
    struct epoll_event ev, events[MAX_EVENTS];
    char buffer[READ_SIZE];
    int nfds, i;

    signal(SIGINT, release);

    // 创建 socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 连接服务器
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
        -1) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 设置 socket 为非阻塞模式
    if (set_nonblocking(sockfd) == -1) {
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 创建 epoll 实例
    if ((epoll_fd = epoll_create1(0)) == -1) {
        perror("epoll_create1");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // 将 sockfd 加入 epoll 实例中
    // 发生读、写事件触发，边缘触发模式。
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    ev.data.fd = sockfd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev) == -1) {
        perror("epoll_ctl");
        close(sockfd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    // 向服务器发送消息
    const char *message = "Hello, server!";
    if (send(sockfd, message, strlen(message), 0) == -1) {
        perror("send");
        close(sockfd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }
    printf("Sent message to server: %s\n", message);

    // 事件循环
    int loops = 1;
    while (1) {
        nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            close(sockfd);
            close(epoll_fd);
            exit(EXIT_FAILURE);
        }
        loops += 1;

        for (i = 0; i < nfds; ++i) {
            if (events[i].events & EPOLLIN) {
                // 处理服务器回显消息
                ssize_t bytes_read =
                    recv(events[i].data.fd, buffer, READ_SIZE - 1, 0);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("Received from server in loop %d: %s\n", loops,
                           buffer);
                } else if (bytes_read == 0) {
                    // 服务器关闭连接
                    printf("Server closed connection\n");
                    close(sockfd);
                    close(epoll_fd);
                    exit(EXIT_SUCCESS);
                } else {
                    perror("recv");
                }
            }

            if (events[i].events & EPOLLOUT) {
                if (loops % 2 == 0) {
                    const char *message = "Another!";
                    if (send(sockfd, message, strlen(message), 0) == -1) {
                        perror("send");
                        close(sockfd);
                        close(epoll_fd);
                        exit(EXIT_FAILURE);
                    }
                    printf("Sent message to server in loop %d: %s\n", loops,
                           message);
                }
            }
        }
    }

    release();
    return 0;
}
