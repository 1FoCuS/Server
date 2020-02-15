#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT ("3490")
#define BACKLOG (10)
#define UNUSADE(x)  ((x)==(x))

void sigchld_handler(int s) {
    UNUSADE(s);
    while(waitpid(-1, NULL, WNOHANG)>0);
}
void *get_in_addr(struct sockaddr* sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main()
{
    int sockfd, newfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    // ************************************************************************************
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;            // my IP

    if ((rv=getaddrinfo(NULL, PORT, &hints, &servinfo))==-1) {
        fprintf(stderr, "bad getaddrinfo: %s\n\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p!=NULL; p=p->ai_next) {

        if((sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror(("socket::server"));
            continue;
        }
        // read
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == 1) {
            perror("setsockopt");
            return 2;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 1) {
            close(sockfd);
            perror("bind");
            continue;
        }
        break;
    }

    if (p==NULL) {
        fprintf(stderr, "server failed to bind\n");
        return 3;
    }
    freeaddrinfo(servinfo);
    // ====================================================================================
    // ************************************************************************************

    if (listen(sockfd, BACKLOG)==-1) {
        perror("listen");
        return 4;
    }
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL)==-1) {
        perror("sigaction");
        return 5;
    }
    // ====================================================================================
    // ************************************************************************************

    printf("Server wait to connections...\n");

    while (1) {
        sin_size = sizeof(their_addr);
        newfd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
        if (newfd==-1){
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr), s, sizeof(s));
        printf("Server got a connection from %s\n", s);


        if (!fork()) {
            close(sockfd);
            if (send(newfd, "my first socket-server", 22, 0) == -1) {
                perror("send");
            }
            close(newfd);
            exit(0);
        }
        close(newfd);
    }
    return 0;
}
