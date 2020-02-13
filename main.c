#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    int status;
    struct addrinfo hints, *p, *res;
    char ipstr[INET6_ADDRSTRLEN];

    /*if (argc!=2)
    {
        fprintf(stderr, "usage: show ip hostname\n");
        return 1;
    }*/
    const char * site = "www.vk.com";

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;            // IPv4 and IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ((status=getaddrinfo(site, NULL, &hints, &res))!=0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 2;
    }
    printf("ip addresses for %s\n\n", site);
    for(p=res; p!=NULL; p=p->ai_next)
    {
        void *addr;
        int port;
        char *ipver;
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
            port = (ipv4->sin_port);
            ipver = "IPv4";
        } else
        {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            port = (ipv6->sin6_port);
            ipver = "IPv6";
        }
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("%s %s %d\n", ipver, ipstr, port);
    }


    freeaddrinfo(res);
    return 0;
}
