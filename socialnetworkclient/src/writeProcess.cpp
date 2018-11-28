/*
 * writeProcess.cpp
 *
 *  Created on: Nov 14, 2018
 *      Author: pournami
 */

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
#include <unordered_map>
#include <iostream>
#include <pthread.h>
#include <time.h>
#include "structures.h"
#include "func_lib.h"
#include "networking.h"

extern const char * getCommand(int enumVal);
extern time_t now;
extern char *error;
extern unsigned int sessionID;

using namespace std;

#define DEBUG	printf
#define ERR_LEN 256

void writeThread(int sock_fd);
int readResponse(int sock_fd, char *buffer, int req_len);
int parsePacket(struct packet *req);
void displayContents(struct packet *resp);
int processResponse(int sock_fd, struct packet *resp);

/*
 * writeThread() - thread to write to stdout
 * sock_fd: socket file descriptor
 */
void writeThread(int sock_fd)
{
	DEBUG("Inside write thread\n");
	int sock_read, sock_write, sock_close;
	struct packet resp;
	int resp_len, ret;
	/* Accept the response persistently*/
	while(1)
	{
		memset(&resp, 0, sizeof(struct packet));
		/* Read server response */
		sock_read = read_socket(sock_fd, resp);
		if(sock_read < 0)
		{
			printf("Error(read_socket)\n");
			return;
		}
		if(!sock_read)
			break;
		/* Parse the packet for valid packet structure */
		ret = parsePacket(&resp);
		if (ret < 0)
		{
			printf("Error (parsePacket): Packet parsing/checking failed\n");
			return;
		}
		/* process the response  */
		ret = processResponse(sock_fd, &resp);
		if (ret < 0)
		{
			printf("Error (processRequest): request processing failed\n");
			return;
		}
	}
}

/*
 * readRequest() - read client request repeatedly
 * sock_fd: slave socket file descriptor
 * buffer: starting point of request structure
 * req_len: length of request structure
 */
int readResponse(int sock_fd, char *buffer, int resp_len)
{
	int sock_read;

	/* Read each request stream repeatedly */
	while (1)
	{
		sock_read = read(sock_fd, buffer, resp_len);
		if (sock_read < 0)
		{
			strerror_r(errno, error, ERR_LEN);
			printf("Error (read): %s\n", error);
			return -1;
		}
		buffer += sock_read;
		resp_len -= sock_read;
		/* Break the loop when the request structure is read completely */
		if (!sock_read || resp_len <= 0)
			break;
	}
	cout<<sock_read<<" bytes read\n";
	return sock_read;
}

/*
 * parsePacket() - parse the packet and validate all fields
 * req: request structure
 * return 0(Valid Packet) -1(Invalid Packet)
 */
int parsePacket(struct packet *resp)
{
	DEBUG("Parsing Packet\n");
	DEBUG("%d | %s | %d | %u\n", resp->content_len, getCommand(resp->cmd_code), resp->req_num, resp->sessionId);
	switch (resp->cmd_code)
	{
	case LOGIN:
		break;
	case POST:
		break;
	case LIST:
		break;
	case SHOW:
		break;
	case NOTIFY:
		break;
	case ACK:
		break;
	default:
		printf("Invalid command, code = %d\n", resp->cmd_code);
		return -1;
	}
	return 0;
}

/*
 * processResponse() - process the response from the server
 * resp: response structure
 * return 0(request processed successfully) -1(request processing failed)
 */
int processResponse(int sock_fd, struct packet *resp)
{
	/*
	 * TODO: in response is login with username invalid msg, then print the error msg
	 */

	if(resp->cmd_code == LIST || resp->cmd_code == SHOW || resp->cmd_code == NOTIFY)
		displayContents(resp);
	else if (resp->cmd_code == LOGIN)
    	sessionID = resp->sessionId;
	else
	{
		printf("Error (processResponse):Invalid Option\n");
		return -1;
	}
	return 0;
}

/*
 * displayContents() - display the contents of the response packet
 * resp: response from server
 */
void displayContents(struct packet *resp)
{
	cout<<resp->contents.rcvd_cnts<<endl;
	return;
}
