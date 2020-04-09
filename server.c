#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT "3490"
#define BACKLOG 3


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


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
        if (p==NULL)
        {
            fprintf(stderr,"server: bind");
            return 2;
        }
        break;
    }
    freeaddrinfo(servinfo);

    if (listen(sockfd, BACKLOG)==-1)
    {
        perror("server: listen");
        exit(1);
    }

    printf("server: waiting connections...\n");
    struct sockaddr_storage their_addr;
    socklen_t their_addr_size;
    struct sigaction sa;
    char s[INET6_ADDRSTRLEN];
    int newfd;
    while(1)
    {
        their_addr_size = sizeof(their_addr);
        if ((newfd = accept(sockfd, (struct sockaddr *)&their_addr, &their_addr_size))==-1)
        {
            perror("server: accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof(s));
        printf("server: got connection from %s\n", s);

        if (!fork())
        {
            close(sockfd);
            if (send(newfd, "Hello, world!", 13, 0) == -1)
            {
                perror("server: send");
            }
            close(newfd);
            exit(0);
        }

        close(newfd);
    }
    return 0;
}
