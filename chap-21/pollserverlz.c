#include "lib/common.h"

#define INIT_SIZE 128

//poll()方法完成的服务端
int main(int argc, char **argv) {
    int serverfd = -1;
    serverfd = socket(PF_INET, SOCK_STREAM, 0);
    if (serverfd < 0) {
        perror("socket() error");
        return -1;
    }

    struct sockaddr_in serversockaddr;
    serversockaddr.sin_family = AF_INET;
    serversockaddr.sin_port = htons(43211);
    serversockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int rt = -1;
    rt = bind(serverfd, (struct sockaddr *) &serversockaddr, sizeof(serversockaddr));
    if (rt < 0) {
        perror("bind() error");
        return -1;
    }

    rt = listen(serverfd, 1024);
    if (rt < 0) {
        perror("listen() error");
        return -1;
    }

    struct pollfd event_set[INIT_SIZE];
    event_set[0].fd = serverfd;
    event_set[0].events = POLLRDNORM;

    int i = -1;
    for (i = 1; i < INIT_SIZE; i++) {
        event_set[i].fd = -1;
    }

    int ready_number = -1;
    struct sockaddr_in client_addr;
    int connected_fd = -1;
    int n = -1;
    char buf[MAXLINE];
    while (1) {
        if ((ready_number = poll(event_set, INIT_SIZE, -1)) < 0) {
            perror("poll() error");
        }
        if (event_set[0].revents & POLLRDNORM) {
            socklen_t client_len = sizeof(client_addr);
            connected_fd = accept(serverfd, (struct sockaddr *) &client_addr, &client_len);
            printf("连接成功,connected_fd = %d\n", connected_fd);
            for (i = 1; i < INIT_SIZE; i++) {
                if (event_set[i].fd < 0) {
                    event_set[i].fd = connected_fd;
                    event_set[i].events = POLLRDNORM;
                    break;
                }
            }

            if (i == INIT_SIZE) {
                printf("can not hold so many clients");
                return -1;
            }
            if (--ready_number <= 0) {
                continue;
            }
        }

        for (i = 1; i < INIT_SIZE; i++) {
            int socket_fd;
            if ((socket_fd = event_set[i].fd) < 0)
                continue;
            if (event_set[i].revents & (POLLRDNORM | POLLERR)) {
                if ((n = read(socket_fd, buf, MAXLINE)) > 0) {
                    if (write(socket_fd, buf, n) < 0) {
                        perror("write error");
                        return -1;
                    }
                } else if (n == 0 || errno == ECONNRESET) {
                    close(socket_fd);
                    event_set[i].fd = -1;
                } else {
                    perror("read error");
                    return -1;
                }

                if (--ready_number <= 0) {
                    break;
                }
            }
        }
    }

}
