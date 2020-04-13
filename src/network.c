#include "network.h"


void check_ip(const char *site)
{
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    err_status = getaddrinfo(site, NULL, &hints, &servinfo);
    if (err_status!=0)
    {
        printf("errro getaddrinfo: %s\n", gai_strerror(err_status));
        return ;
    }

    char data[INET6_ADDRSTRLEN];
    for(p=servinfo; p!=NULL; p=p->ai_next)
    {
        void *address;
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
            address = &ipv4->sin_addr;
        } else 
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)p->ai_addr;
            address = &ipv6->sin6_addr;
        }
        inet_ntop(p->ai_family, address, data, INET6_ADDRSTRLEN);
        printf("%s\n", data);
    }
}

int init_general(const char*node, const char*service, ptr_init_f func)
{
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    err_status = getaddrinfo(node, service, &hints, &servinfo);
    if (err_status!=0)
    {
        printf("getaddr failed: %s\n", gai_strerror(err_status));
        return -1;
    }
    int sockfd;
    for(p=servinfo; p!=NULL; p=p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd==-1)
        {
            perror("socket failed");
            continue;
        }
        err_status = func(sockfd, p->ai_addr, p->ai_addrlen);
        
        if (err_status==-1)
        {
            perror("ptr function failed");
            continue;
        }
        break;
    }
    if (p==NULL)
    {
        perror("addrinfo failed");
        sockfd = -1;
    }
    freeaddrinfo(servinfo);

    return sockfd;
}

int init_server()
{
    int listener = init_general(NULL, PORT, bind);
    if (listener==-1)
    {
        perror("init server failed");
        exit(1);
    }

    err_status = listen(listener, BACKLOG);
    if (err_status==-1)
    {
        perror("listen failed");
        exit(1);
    }
    printf("server wait connections...\n");

    return listener;
}

int run_client()
{
    int sock_fd = init_general("zverek", PORT, connect);
    if (sock_fd==-1)
    {
        perror("init general failed");
        exit(1);
    }

    char message[MAX_LEN];
    while (1)
    {
        create_mes_send(message, MAX_LEN, 5);
        int len = strlen(message);
        if (len) 
        {
            int nbytes = send(sock_fd, message, len, 0);
            if (nbytes<=0)
            {
                printf("close server\n");
                break;
            }
        }
        int nbytes = read_message(sock_fd, message, MAX_LEN, 0);
        if (nbytes>0) printf("> %s\n", message);    
    }
    close(sock_fd);
    printf("close client\n");
}

void run_server()
{
    int listener = init_server();
    struct sockaddr_storage address;
    socklen_t len_addr = sizeof(address);

    
    
    fd_set master, readfds;
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(listener, &master);
    int fdmax = listener;
    int new_fd;

    while (1)
    {
        //printf("start while\n");
        readfds = master;
        if (select(fdmax+1, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(4);
        }
        for(int i=0; i<fdmax+1; ++i)
        {
            if (FD_ISSET(i, &readfds))
            {
                if (i==listener)
                {
                    new_fd = accept(listener, (struct sockaddr *)&address, &len_addr);
                    if (new_fd==-1)
                    {
                        perror("accept failed");
                        continue;
                    }
                    else
                    {
                        FD_SET(new_fd, &master);
                        if (new_fd>fdmax) fdmax = new_fd;
                    }
                    char buf[INET6_ADDRSTRLEN];
                    inet_ntop(address.ss_family, get_in_addr((struct sockaddr *)&address), buf, sizeof(buf));
                    printf("new connection from %s, fd=%d\n", buf, new_fd);
                }
                else
                {
                    char message[MAX_LEN];
                    int nbytes = recv(i, message, MAX_LEN, 0);
                    message[nbytes] = '\0';
                    if (nbytes>0) printf("> %s\n", message);
                    if (nbytes<=0)
                    {
                        perror("recv");
                        close(i);
                        FD_CLR(i, &master);
                    }
                    else
                    {
                        for(int j=0; j<fdmax+1; ++j)
                        {
                            if (FD_ISSET(j, &master))
                            {
                                //printf("try send %d fd\n", j);
                                if (j!=listener && i!=j)
                                {
                                    //printf("send mes to %d fd\n", j);
                                    int jbytes = send(j, message, nbytes, 0);
                                    if (jbytes<0)
                                    {
                                        perror("send");
                                    }
                                }
                                
                            }
                        }
                        //printf("end send mes to fds\n");
                    }
                    //printf("test\n");
                }
                
            }
        }
        //printf("end while\n");
    }
    
}

void* get_in_addr(struct sockaddr * sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    else 
    {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

void create_mes_send(char *str, int nbytes, int sec)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);
    int maxfds = STDIN;
    select(maxfds+1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(STDIN, &readfds))
    {
        int n = read(STDIN, str, nbytes);
        str[n-1] = '\0';
    }
    else 
    {
        str[0] ='\0';
    }
    //printf("create mess: %s\n", str);
}

int read_message(int fd, char *str, int nbytes, int sec)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    int maxfds = fd;
    select(maxfds+1, &readfds, NULL, NULL, &tv);

    if (FD_ISSET(fd, &readfds))
    {
        nbytes = recv(fd, str, nbytes, 0);
        str[nbytes] = '\0';
    }
    else
    {
        str[0] = '\0';
        nbytes = 0;
    }
    return nbytes;
}