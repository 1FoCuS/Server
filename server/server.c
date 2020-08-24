#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>

#define MAXLINE 256

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    int listenfd;
    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in servinfo;
    memset(&servinfo, 0, sizeof(servinfo));
    servinfo.sin_family = AF_INET;
    servinfo.sin_addr.s_addr = htonl(INADDR_ANY);
    servinfo.sin_port = htons(7778);
    if ( bind(listenfd, (const struct sockaddr*)&servinfo, sizeof (servinfo)) == -1 ) {
        perror("bind");
        return 2;
    }
    listen(listenfd, 3);

    socklen_t sock_addr_size = sizeof (struct sockaddr);
    fd_set master, readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(listenfd, &master);
    int fdmax = listenfd;
    int newfd;

    while (1) {

        readfds = master;
        if (select(fdmax+1, &readfds, NULL, NULL, NULL)==-1) {
            perror("select");
            return 3;
        }
        for (int fd=0; fd<fdmax+1; ++fd) {
            if (FD_ISSET(fd, &readfds)) {
                if (fd == listenfd) {
                    newfd = accept(listenfd, (struct sockaddr*)&servinfo, &sock_addr_size);
                    if (newfd==-1) {
                        perror("accept failed");
                        continue;
                    }
                    else {
                        FD_SET(newfd, &master);
                        if (newfd>fdmax) fdmax = newfd;
                    }
                    printf("new connection\n");
                }
                else {
                    char message[256];
                    int nbytes = recv(fd, message, 256, 0);
                    message[nbytes] = '\0';
                    if (nbytes>0) {
                        printf("> %s", message);
                        for(int curfd = 0; curfd<fdmax+1; ++curfd) {
                            if(FD_ISSET(curfd, &master) && curfd!=listenfd && curfd!=fd) {
                                int jbytes = send(curfd, message, nbytes, 0);
                                if (jbytes<0) {
                                    perror("send");
                                }
                            }
                        }
                    }
                    else {
                        perror("close connection");
                        close(fd);
                        FD_CLR(fd, &master);
                    }
                }

            } // close main if
        } // close for
    } // close while
    close(listenfd);
    return 0;
}
