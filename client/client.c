#include <stdio.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>

#define MAXLINE (256)
#define CODE_ERROR (-1)
#define CODE_SUCCESS (0)

void* handler_read_msgs(void *args) {
//    printf("handler_read_msgs()\n");
    int sockfd = *(int *)args;
    char recvline[MAXLINE];
    int n;
    while( (n=recv(sockfd, recvline, MAXLINE, 0)) >0) {
        recvline[n] = '\0';
        if (fputs(recvline, stdout) == EOF) {
            perror("fputs error");
            return (void*)CODE_ERROR;
        }
    }
    if (n<0) {
        perror("read error");
        return (void*)CODE_ERROR;
    }
    else {
        printf("success close client\n");
    }

    return CODE_SUCCESS;
}

void* handler_write_msgs(void *args) {
    int sockfd = *(int *)args;
    char buf[MAXLINE];
    int nbytes;
    while ( (nbytes=read(STDIN_FILENO, buf, MAXLINE)) >0) {
        buf[nbytes] = '\0';
        if ( (nbytes = send(sockfd, buf, nbytes, 0)) == CODE_ERROR ) {
            perror("error send");
            return (void*)CODE_ERROR;
        }
    }
    return CODE_SUCCESS;
}
int main(int argc, char *argv[])
{
    if (argc!=2) {
        perror("argc!=2");
        return 1;
    }

    int sockfd;
    if ( (sockfd=socket(AF_INET, SOCK_STREAM, 0)) == CODE_ERROR) {
        perror("socket ");
        return 1;
    }

    struct sockaddr_in servinfo;
    memset(&servinfo, 0, sizeof (servinfo));
    servinfo.sin_family = AF_INET;
    servinfo.sin_port = htons(7777);
    if ( inet_pton(AF_INET, argv[1], &servinfo.sin_addr) == CODE_ERROR) {
        perror("inet pron ");
        return 2;
    }
    if (connect(sockfd, (const struct sockaddr*)&servinfo, sizeof (servinfo)) == CODE_ERROR) {
        perror("inet pron ");
        return 3;
    }

    pthread_t th_read_msgs;
    int status = pthread_create(&th_read_msgs, NULL, handler_read_msgs, (void*)&sockfd);
    if (status != CODE_SUCCESS) {
        printf("error thread read\n");
    }

    pthread_t th_write_msgs;
    status = pthread_create(&th_write_msgs, NULL, handler_write_msgs, (void*)&sockfd);
    if (status != CODE_SUCCESS) {
        printf("error thread write\n");
    }

    status = pthread_join(th_read_msgs, (void **)&status);
    status = pthread_join(th_write_msgs, (void **)&status);

    return 0;
}
