#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

// #define SERV_PORT 5000
#define LISTENQ 10
#define MAX_INPUT_SIZE 100
