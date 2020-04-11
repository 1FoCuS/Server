#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
int main()
{
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo("www.yandex.com", NULL, &hints, &servinfo);
    if (status!=0)
    {
        printf("error getaddr %s\n", gai_strerror(status));
        return 1;
    }
    struct addrinfo *p;
    char data[INET6_ADDRSTRLEN];
    void *addr;

    for(p=servinfo; p!=NULL; p=p->ai_next)
    {

        if (p->ai_family== AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &ipv4->sin_addr;
            printf("IPv4 ");
        }
        else
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &ipv6->sin6_addr;
            printf("IPv6 ");
        }
        inet_ntop(p->ai_family, addr, data, sizeof(data));
        printf("%s\n", data);
    }
    freeaddrinfo(servinfo);
    return 0;
}
