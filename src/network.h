#ifndef __NETWOK__
#define __NETWORK__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

#define PORT "3490"
#define BACKLOG 5
#define MAX_LEN 1024
#define STDIN 0

static int err_status;

typedef int (*ptr_init_f)(int, const struct sockaddr*, socklen_t);

void check_ip(const char *);

int init_general(const char*, const char*, ptr_init_f);
int init_server();
int init_client();

void run_server();
void* get_in_addr(struct sockaddr *);
void create_message(char *, int );


#endif // !__NETWOK__
