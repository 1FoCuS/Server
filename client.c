#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT ("3491")
#define MAXDATASIZE 256
#define UNUSADE(x)  ((x)==(x))

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void init_name(void) {
    char host_name[MAXDATASIZE];
    int er;
    if ((er=gethostname(host_name, MAXDATASIZE))==-1) {
        perror("error name");
        exit(1);
    }
    printf("Host-name: %s\n", host_name);
}

struct addrinfo* init_client(void) {
    init_name();
    struct addrinfo hints, *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int rv;
    if ((rv=getaddrinfo(NULL, PORT, &hints, &server_info))==-1) {
        fprintf(stderr, "bad getaddrinfo: %s\n\n", gai_strerror(rv));
        exit(1);
    }

    return server_info;
}
int init_socket_client(struct addrinfo* servinfo)
{
    struct addrinfo* p;
    int sockfd;
    char s[INET6_ADDRSTRLEN];

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
                perror("client: socket");
                continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return -1;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo);

    return sockfd;
}
void run(int sockfd)
{
    char message[MAXDATASIZE];
    int numbytes;

    fprintf(stdout,"client: ");
    gets(message);
    message[sizeof(message)-1] ='\0';

    if ((numbytes = send(sockfd, message, sizeof(message), 0)) == -1)
    {
        perror("send");
        exit(1);
    }

    if ((numbytes = recv(sockfd, message, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    message[numbytes]= '\0';
    printf("server: %s\n", message);
}

int main(int argc, char *argv[]) {
    UNUSADE(argv);
    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
    struct addrinfo* servinfo = init_client();

    int sockfd = init_socket_client(servinfo);

    while (1) {
        run(sockfd);
    }

    close(sockfd);
    return 0;
}
