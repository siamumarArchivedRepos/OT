#ifndef TCPIP
#define TCPIP

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <netinet/tcp.h> 
#include <gmpxx.h>

#define BASE 8

int server_init(int );
int client_init(char*, int);
void send_bgInt(int,  mpz_t);
void recv_bgInt(int,  mpz_t);

#endif