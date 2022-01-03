#ifndef __HEADER_H__
#define __HEADER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define PORT            (8080)
#define BUFF_SIZE       (100)
#define SUCCESS         (0)
#define ERROR           (1)

typedef struct sockaddr sockaddr;

#endif