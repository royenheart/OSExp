#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define MAXLINE 80

const char *client_path = "client.socket";
const char *server_path = "server.socket";

int main(int argc, char *argv[]) {
    struct sockaddr_un cliun, serun;
    int len;
    char buf[100];
    int sockfd, n;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("client socket error");
        exit(EXIT_FAILURE);
    }

    // 一般显式调用bind函数，以便服务器区分不同客户端
    memset(&cliun, 0, sizeof(cliun));
    cliun.sun_family = AF_UNIX;
    strcpy(cliun.sun_path, client_path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(cliun.sun_path);
    unlink(cliun.sun_path);
    if (bind(sockfd, (struct sockaddr *)&cliun, len) < 0) {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    memset(&serun, 0, sizeof(serun));
    serun.sun_family = AF_UNIX;
    strcpy(serun.sun_path, server_path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
    if (connect(sockfd, (struct sockaddr *)&serun, len) < 0) {
        perror("connect error");
        exit(EXIT_FAILURE);
    }

    while (fgets(buf, MAXLINE, stdin) != NULL) {
        write(sockfd, buf, strlen(buf));
        n = read(sockfd, buf, MAXLINE);
        if (n < 0) {
            std::cout << "the other side has been closed." << std::endl;
            break;
        } else {
            write(STDOUT_FILENO, buf, n);
            if (strcmp(buf, "Connect shudown!\n") == 0) {
                break;
            }
        }
        memset(buf, 0, sizeof(buf));
    }
    close(sockfd);
    return EXIT_SUCCESS;
}
