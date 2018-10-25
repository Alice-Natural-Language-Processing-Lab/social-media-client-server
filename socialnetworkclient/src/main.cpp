/*
 * main.cpp
 *
 *  Created on: Oct 24, 2018
 *      Author: pournami
 */

#include "structures.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <string>
#include <iostream>

using namespace std;

#define DEBUG printf

int sendRequest(int sock_fd, struct request *req);
int getAddrInfo(string host, string port, struct addrinfo **serv_info);
int processClient(string servername, string serverport);
int tcpSocket(string host, string serverport);
int getUserRequest(int sock_fd);

int main(int argc, char *argv[])
{
	string servername = "localhost";
	string serverport = "12345";
	int ret = 0;

	switch (argc)
	{
	case 1:
			break;
	case 2:
			servername = argv[1];
			break;
	case 3:
			servername = argv[1];
			serverport = argv[2];
			break;
	default:
			printf("Error: Usage is ./[executable] [hostname] [port]\n");
			return -1;
	}
	ret = processClient(servername, serverport);
	if (ret < 0)
	{
		printf("Error (processClient): Client processing failed\n");
		return -1;
	}
	return 0;
}

/**
 * processClient - process the client for social network
 * servername: the server address
 * serverport: port number of server
 * returns 0(success) -1(error)
 */
int processClient(string servername, string serverport)
{
	int sock_fd, sock_close;
	int ret	= 0;

	sock_fd = tcpSocket(servername, serverport);
	if (sock_fd < 0)
	{
		printf("Error (tcpSocket): socket creation not successful\n");
		return -1;
	}
	ret = getUserRequest(sock_fd);
	if (ret < 0)
	{
		printf("Error (getUserRequest): user request not successful\n");
		return -1;
	}
	sock_close = close(sock_fd);
	if (sock_close < 0)
	{
		printf("Error (close): %s\n", strerror(errno));
		return -1;
	}
	DEBUG("Socket closed \n");
	return 0;
}

/**
 * tcpSocket - to create a tcp client socket
 * host: server host address
 * serverport: port number of server
 * returns 0(success) -1(error)
 */
int tcpSocket(string host, string serverport)
{
	int addr_info, sock_fd, sock_conv, sock_conn, sock_read, sock_write, sock_shut, sock_close, client_info;
	struct addrinfo hints, *serv_info, *rp;
	struct sockaddr_in sock_addr;

	/* Get server info */
	addr_info = getAddrInfo(host, serverport, &serv_info);
	if (addr_info != 0)
	{
		printf("Error (getaddrinfo): %s\n", gai_strerror(addr_info));
		return -1;
	}
	/* create a client socket and connect the socket from the list of addrinfo*/
	for (rp = serv_info; rp != NULL; rp = rp->ai_next)
	{
		sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sock_fd < 0)
			continue;
		DEBUG("Socket created\n");
		sock_conn = connect(sock_fd, rp->ai_addr, rp->ai_addrlen);
		if (sock_conn != -1)
			break;
		close(sock_fd);
	}
	if (rp == NULL)
	{
		printf("No address in the list was a success\n");
		return -1;
	}
	freeaddrinfo(serv_info);
	DEBUG("Socket connected \n");
	return sock_fd;
}

/**
 * getUserRequest - collect user request and send it to server
 * sock_fd: socket file descriptor
 * returns 0(success) -1(error)
 */
int getUserRequest(int sock_fd)
{
	char *user_input = (char *)malloc(sizeof(char) * CMD_LEN);
	long unsigned int req_size;
	char *ptr;
	struct request *req = (struct request *)malloc(sizeof(struct request));
	int ret;

	while (1)
	{
		memset(req, 0, sizeof(struct request));
		DEBUG("Enter command: ");
		getline(&user_input, &req_size, stdin);
		strcpy(req->command, strtok(user_input, "\n"));
		ret = sendRequest(sock_fd, req);
		if (ret < 0)
		{
			printf("Error (sendRequest): Request sending failed\n");
			return -1;
		}
		if (!strcmp(req->command, "logout"))
			break;
	}
	DEBUG("Logging out\n");
	return 0;
}

/**
 * sendRequest - send the request to server
 * sock_fd: socket file descriptor
 * req: request structure to store request
 * returns 0(success) -1(error)
 */
int sendRequest(int sock_fd, struct request *req)
{
	int sock_send, sock_read, sock_shut;
	int req_len = sizeof(struct request);

	/* Send the request to server */
	sock_send = send(sock_fd, req, req_len, 0);
	if (sock_send < 0)
	{
		printf("Error (send):%s\n", strerror(errno));
		return -1;
	}
	DEBUG("Request sent\n");
	return 0;
}

/**
 * getAddrInfo - get end point information
 * host: host address of server
 * port: port number of server
 * serv_info: to store the server information
 * returns addr_info
 */
int getAddrInfo(string host, string port, struct addrinfo **serv_info)
{
	struct addrinfo hints;
	int addr_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= 0;
	hints.ai_flags		= 0;
	addr_info		= getaddrinfo(host.c_str(), port.c_str(), &hints, serv_info);
	return addr_info;
}

