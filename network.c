#include "network.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

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