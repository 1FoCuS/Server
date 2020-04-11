#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define STDIN 0
#define MAX_LEN 1024

int main()
{
    struct timeval tv;
    fd_set readfds, master;

    FD_ZERO(&master);
    FD_SET(STDIN, &master);

    char buffer[MAX_LEN];
    while (1) {
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        readfds = master;
        select(STDIN+1, &readfds, NULL, NULL, &tv);

        if (FD_ISSET(STDIN, &readfds))
        {
            int n = read(STDIN, buffer, MAX_LEN);
            if (n>0)
            {
                buffer[n-1] = '\0';
                printf("read: %s\n", buffer);
            }

        }
        else
        {
            printf("waiting\n");
        }
    }
    return 0;
}
