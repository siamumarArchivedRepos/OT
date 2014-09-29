#include <iostream>
#include<string.h>
#include <gmpxx.h>
#include "tcpip.h"
#include <errno.h>
using namespace std; 

int f;

int server_init(int port){ 
	int connfd, listenfd;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0){
		cout << strerror(errno) << endl;
		return -1;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		cout << strerror(errno) << endl;
		return -1;
	}
	listen(listenfd,5);
	clilen = sizeof(cli_addr);
	cout << "Wait for client" << endl;
	connfd = accept(listenfd, (struct sockaddr *) &cli_addr, &clilen);
	if (connfd < 0)
	{
		cout << strerror(errno) << endl;
		return -1;
	}
	
	cout << "Connected" << endl;
	int flag = 1;
	int result = setsockopt(connfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)); 
	if (result < 0){
		cout << strerror (errno) << endl;
		return -1;
	}
	return connfd;
}

int client_init(char* ip, int port){
	int sockfd;
	struct hostent *server;
	struct sockaddr_in serv_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cout << strerror(errno) << endl;
		return -1;
	}
	server = gethostbyname(ip);
	if (server == NULL)
	{
		cout << "ERROR, no such host" << endl;
		return -1;
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		cout << strerror(errno) << endl;
		return -1;
	}
	cout << "Connected" << endl;
	int flag = 1;
	int result = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)); 
	if (result < 0){
		cout << strerror (errno) << endl;
		return -1;
	}	
	return sockfd;
}

void send_bgInt(int sock, mpz_t bigI){	
	size_t len;	
	char * conv_str = (char *) mpz_export (NULL, &len, 1, 1, 1, 0, bigI);
	write(sock, &len, sizeof(size_t));	
	write(sock, conv_str, len);	
	delete [] conv_str;
}

void recv_bgInt(int sock, mpz_t bigI){
	size_t len;
	read(sock, &len, sizeof(size_t));		
	char* conv_str = new char[len];
	read(sock, conv_str, len);
	mpz_import (bigI, len, 1, 1, 1, 0, conv_str);
	delete [] conv_str;
}