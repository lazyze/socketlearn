#include "lib/common.h"

#define  MAXLINE     1024


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: select01lz <IPaddress>\n");
        return -1;
    }
    int clientfd = -1;
    //创建套接字
    clientfd = socket(PF_INET, SOCK_STREAM, 0);
    if (clientfd < 0) {
        perror("socket() error");
        return -1;
    }
    struct sockaddr_in serversockaddr;
    serversockaddr.sin_family = AF_INET;
    serversockaddr.sin_port = htons(43211);
    //serversockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(AF_INET, "127.0.0.1", &serversockaddr.sin_addr);

    int connect_rt = -1;
    connect_rt = connect(clientfd, (struct sockaddr *) &serversockaddr, sizeof(serversockaddr)); 
    if (connect_rt < 0) {
        perror("connect() error");
        return -1;
    }

    printf("客户端连接成功\n");

    fd_set allfds;
    fd_set allfds_rt;
    FD_ZERO(&allfds);
    FD_SET(0, &allfds);
    FD_SET(clientfd, &allfds);

    char send_line[MAXLINE];
    char recv_line[MAXLINE];
    int n = -1;
    int num = 0;
    int outnum = 0;
    while (1) {
        outnum++;
        allfds_rt = allfds;
        int rt = select(clientfd + 1, &allfds_rt, NULL, NULL, NULL);
        if (rt <= 0) {
            perror("select() error");
            return -1;
        }

        

        if (FD_ISSET(clientfd, &allfds_rt)) {
            printf("套接字路有事件发生\n");
            num++;
            printf("num = %d, outmum = %d", num, outnum);
            n = read(clientfd, recv_line, MAXLINE);
            if (n < 0) {
                perror("read error");
            } else if (n == 0) {
                printf("server terminated \n");
            }
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        if (FD_ISSET(0, &allfds_rt)) {
            printf("标准输入路有事件发生\n");
            num++;
            printf("num = %d, outmum = %d", num, outnum);
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                int i = strlen(send_line);
                if (send_line[i - 1] == '\n') {
                    send_line[i - 1] = 0;
                }

                printf("now sending %s\n", send_line);
                int rt = write(clientfd, send_line, strlen(send_line));
                if (rt < 0) {
                    perror("write() failed");
                }
                printf("send bytes: %d\n", rt);
            }
        }


    }
    return 0;
}  

    
