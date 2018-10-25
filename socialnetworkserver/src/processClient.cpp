/*
 * processClient.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */
#include "func_lib.h"
#include "structures.h"

using namespace std;

#define ERR_LEN			256
#define DEBUG	printf

void handleClient(int sock_fd);
void readRequest(int sock_fd, char *buffer, int req_len);
int parsePacket(struct request *req);
int sessionValidity(struct request *req);
int permissionValidity(struct request *req);

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
		if (!strcmp(req->command, ""))
			break;
		DEBUG("Request received: %s\n", req->command);
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
			return;
		}

		/* Validate permissions of the client */
		ret = permissionValidity(req);
		if (ret < 0) /* permission validity could not be established */
		{
			printf("Error (permissionValidity): permission validity could not be established\n");
			/* TODO : SEND WARNING MESSAGE TO CLIENT "NOT LOGGED IN/PERMISSION DENIED" */
			terminateClient(sock_fd);
			return;
		}

		/* process the request for appropriate permissions */
		ret = processRequest(req);
		if (ret < 0)
		{
			printf("Error (processRequest): request processing failed\n");
			return;
		}
		/* TODO : function call to file where request processing is taken care of*/

		/* TODO : terminate client connection on appropriate request */
	}
	DEBUG("Client connection closed\n");
	close(sock_fd);
	return;

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
	return;
}

/*
 * parsePacket() - parse the packet and validate all fields
 * req: request structure
 * return 0(Valid Packet) -1(Invalid Packet)
 */
int parsePacket(struct request *req)
{
	DEBUG("Parsing Packet\n");
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
	DEBUG("Checking session validity\n");
	/* TODO : Complete the function */
	return 0;
}

/*
 * permissionValidity() - validate the client session
 * req: request structure
 * return 0(Valid session) -1(Invalid session)
 */
int permissionValidity(struct request *req)
{
	DEBUG("Checking permission for request\n");
	/* TODO : Complete the function */
	return 0;
}
