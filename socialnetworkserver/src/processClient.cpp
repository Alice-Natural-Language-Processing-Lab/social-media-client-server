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
int parsePacket(struct packet *req);
int sessionValidity(struct packet *req);
int permissionValidity(struct packet *req, int valid);

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
	struct packet *req = (struct packet *)malloc(sizeof(struct packet));
	char *error = (char *)malloc(sizeof(char) * ERR_LEN);
	int threadId = pthread_self();
	char *buffer = (char *)req;
	int req_len, ret;

	/* Accept the request persistently*/
	while(1)
	{
		memset(req, 0, sizeof(struct packet));
		req_len = sizeof(struct packet);
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
			return;
		}

		/* process the request with appropriate permissions */
		ret = processRequest(req);
		if (ret < 0)
		{
			printf("Error (processRequest): request processing failed\n");
			return;
		}

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
int parsePacket(struct packet *req)
{
	DEBUG("Parsing Packet\n");
	/* TODO : Complete the function */
	if (req->cmd_code != LOGIN && req->cmd_code != LOGOUT && req->cmd_code != POST && req->cmd_code != LIST)
	{
		printf("Invalid command %d", req->cmd_code);
		return -1;
	}
	return 0;
}

/*
 * sessionValidity() - validate the client session
 * req: request structure
 * return 0(Valid session) -1(Invalid session)
 */
int sessionValidity(struct packet *req)
{
	DEBUG("Checking session validity\n");
	/* TODO : Complete the function */
	int ret = 0;
	int valid = 0;
	/* check session validity and modify variable valid*/

	/* Validate permissions of the client */
	ret = permissionValidity(req, valid);
	if (ret < 0)
	{
		printf("User do not have permission to execute the command\n");
		return -1;
	}
	return ret;
}

/*
 * permissionValidity() - validate the client session
 * req: request structure
 * return 0(Valid permission) -1(Invalid permission)
 */
int permissionValidity(struct packet *req, int valid)
{
	DEBUG("Checking permission for request\n");
	/* TODO : Complete the function */
	int ret = 0;
	if (!valid)
	{
		if (req->cmd_code != LOGIN)
			return -1;
	}
	return 0;
}
