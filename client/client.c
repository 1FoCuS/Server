#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 256

int main(int argc, char *argv[])
{
    int sockfd;

    if (argc!=2) {
        perror("argc!=2");
        return 1;
    }

    if ( (sockfd=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket ");
        return 1;
    }

    struct sockaddr_in servinfo;
    memset(&servinfo, 0, sizeof (servinfo));
    servinfo.sin_family = AF_INET;
    servinfo.sin_port = htons(7777);
    if ( inet_pton(AF_INET, argv[1], &servinfo.sin_addr) == -1) {
        perror("inet pron ");
        return 2;
    }
    if (connect(sockfd, (const struct sockaddr*)&servinfo, sizeof (servinfo)) == -1) {
        perror("inet pron ");
        return 3;
    }

    int n;
    char recvline[MAXLINE];
    while( (n=read(sockfd, recvline, MAXLINE)) >0) {
        recvline[n] = '\0';
        if (fputs(recvline, stdout) == EOF) {
            perror("fputs error");
            return 4;
        }
    }
    if (n<0) {
        perror("read error");
        return 5;
    }
    return 0;
}
