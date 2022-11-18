#include <stdlib.h>  
#include <iostream>
#include <cstddef>
#include <sys/socket.h>  
#include <sys/un.h>  
#include <errno.h>  
#include <cstring>  
#include <unistd.h>  
#include <ctype.h>   
 
#define MAXLINE 80  

const char* socket_path = "server.socket";  
 
int main(int argc, char *argv[]) {  
    struct sockaddr_un serun, cliun;  
    socklen_t cliun_len;  
    int listenfd, connfd, size;  
    char buf[MAXLINE];  
    int i, n;  
 
    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {  
        perror("socket error");  
        exit(EXIT_FAILURE);  
    }  
 
    memset(&serun, 0, sizeof(serun));  
    serun.sun_family = AF_UNIX;  
    strcpy(serun.sun_path, socket_path);  
    size = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);  
    unlink(socket_path);  
    if (bind(listenfd, (struct sockaddr *)&serun, size) < 0) {  
        perror("bind error");  
        exit(EXIT_FAILURE);  
    }  
    std::cout << "UNIX domain socket bound" << std::endl;
      
    if (listen(listenfd, 20) < 0) {  
        perror("listen error");  
        exit(EXIT_FAILURE);          
    }  
    std::cout << "Accepting connections ..." << std::endl; 
 
    while (true) {  
        cliun_len = sizeof(cliun);         
        if ((connfd = accept(listenfd, (struct sockaddr *)&cliun, &cliun_len)) < 0){  
            perror("accept error");  
            continue;  
        }
          
        while (true) {  
            n = read(connfd, buf, sizeof(buf));  
            if (n < 0) {  
                perror("read error");
                break;  
            } else if (n == 0) {  
                std::cout << "Client sent EOF. Client shutdown" << std::endl;
                break;
            }  
              
            std::cout << "received: " << buf; 
 	    
            if (strcmp(buf, "exit\n") == 0) {
                const char* server_down = "Connect shudown!\n";
                std::cout << "client exit!" << std::endl;
                write(connfd, server_down, strlen(server_down));
                memset(buf, 0, sizeof(buf));
                break;
            }

            for(i = 0; i < n; i++) {
                buf[i] = toupper(buf[i]);  
            }
            write(connfd, buf, n);
            memset(buf, 0, sizeof(buf));
        }
        close(connfd);
    }
    close(listenfd);  
    return 0;  
}
