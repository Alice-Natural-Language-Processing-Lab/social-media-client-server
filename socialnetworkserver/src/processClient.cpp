/*
 * processClient.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */
#include "structures.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

#define ERR_LEN 256
#define DEBUG   printf

void handleClient(int sock_fd);
void readRequest(int sock_fd, char *buffer, int req_len);
int parsePacket(struct request *req);
int sessionValidity(struct request *req);
void terminateClient(int slave_fd);

/*
 * client initial setup
 */

/*
 * handleClient() - handle each client connection
 * sock_fd: slave socket file descriptor
 */
void handleClient(int sock_fd)
{
	int sock_read, sock_write, sock_close;
	struct request *req = (struct request *)malloc(sizeof(struct request));
	char *error = (char *)malloc(sizeof(char) * ERR_LEN);
	int threadId = pthread_self();
	char *buffer = (char *)req;
	int req_len, ret;

	/* Accept the request persistently*/
	while(1)
	{
		memset(req, 0, sizeof(struct request));
		req_len = sizeof(struct request);
		/* Read client request */
		readRequest(sock_fd, buffer, req_len);

		/* Parse the packet for valid packet structure */
		ret = parsePacket(req);
		if (ret < 0)
		{
			printf("Error (parsePacket): Packet parsing/checking failed\n");
			terminateClient(sock_fd);
		}

		/* Validate session of the client */
		ret = sessionValidity(req);
		if (ret < 0) /* session validity could not be established */
		{
			printf("Error (sessionValidity): session validity could not be established\n");
			/* TODO : SEND WARNING MESSAGE TO CLIENT "NOT LOGGED IN/SESSION EXPIRED" */
			terminateClient(sock_fd);
		}

		/* process the request for appropriate permissions */
		/* TODO : function call to file where request processing is taken care of*/

		/* TODO : terminate client connection on appropriate request */
	}

}

/*
 * readRequest() - read client request repeatedly
 * sock_fd: slave socket file descriptor
 * buffer: starting point of request structure
 * req_len: length of request structure
 */
void readRequest(int sock_fd, char *buffer, int req_len)
{
	int sock_read;
	char *error = (char *)malloc(sizeof(char) * ERR_LEN);

	/* Read each request stream repeatedly */
	while (1)
	{
		sock_read = read(sock_fd, buffer, req_len);
		if (sock_read < 0)
		{
			strerror_r(errno, error, ERR_LEN);
			printf("Error (read): %s\n", error);
			return;
		}
		buffer += sock_read;
		req_len	-= sock_read;
		/* Break the loop when the request structure is read completely */
		if (!sock_read || req_len <= 0)
			break;
	}
	DEBUG("Request received\n");
	return;
}

/*
 * parsePacket() - parse the packet and validate all fields
 * req: request structure
 * return 0(Valid Packet) -1(Invalid Packet)
 */
int parsePacket(struct request *req)
{
	/* TODO : Complete the function */

	return 0;
}

/*
 * sessionValidity() - validate the client session
 * req: request structure
 * return 0(Valid session) -1(Invalid session)
 */
int sessionValidity(struct request *req)
{
	/* TODO : Complete the function */
	return 0;
}
