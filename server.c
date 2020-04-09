#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT "3490"

int main()
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int rv;
    struct addrinfo *servinfo;
    if ((rv=getaddrinfo(NULL, PORT, &hints, &servinfo))!=0)
    {
        fprintf(stderr, "line #%d: getaddrinfo %s\n",__LINE__,  gai_strerror(rv));
        return 1;
    }

    int sockfd;
    int yes=1;
    for(struct addrinfo *p=servinfo; p!=NULL; p=p->ai_next)
    {
        if ((sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int))==-1)
        {
            perror("server: setsocketopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen)==-1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
/*
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    struct sigaction sa;


    char s[INET6_ADDRSTRLEN];
*/
    printf("Hello Server!\n");
    return 0;
}
