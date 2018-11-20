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
extern const char * getCommand(int enumVal);
//extern FILE *logfile;
extern time_t now;
extern char *error;
extern unsigned int sessionID;

using namespace std;

#define DEBUG	printf
#define ERR_LEN 256

void writeThread(int sock_fd);
void readResponse(int sock_fd, char *buffer, int req_len);
int parsePacket(struct packet *req);
void displayContents(struct packet *resp);
int processResponse(int sock_fd, struct packet *resp);

void writeThread(int sock_fd)
{
	DEBUG("Inside write thread\n");
	int sock_read, sock_write, sock_close;
	struct packet *resp = (struct packet *)malloc(sizeof(struct packet));
	char *buffer = (char *)resp;
	int resp_len, ret;
	/* Accept the response persistently*/
	while(1)
	{
		memset(resp, 0, sizeof(struct packet));
		resp_len = sizeof(struct packet);
		/* Read server response */
		readResponse(sock_fd, buffer, resp_len);
		if(!strlen(buffer))
			break;
		/* Parse the packet for valid packet structure */
		ret = parsePacket(resp);
		if (ret < 0)
		{
			printf("Error (parsePacket): Packet parsing/checking failed\n");
			return;
		}
		now = time(NULL);
		//fprintf(logfile, "%s: Response received:\n", strtok(ctime(&now), "\n"));
		//fprintf(logfile, "%d | %s | %d | %d\n", resp->content_len, getCommand(resp->cmd_code), resp->req_num, resp->sessionId);
		/* process the response  */
		ret = processResponse(sock_fd, resp);
		if (ret < 0)
		{
			printf("Error (processRequest): request processing failed\n");
			return;
		}
		//fflush(logfile);
	}
	//fclose(logfile);

}

/*
 * readRequest() - read client request repeatedly
 * sock_fd: slave socket file descriptor
 * buffer: starting point of request structure
 * req_len: length of request structure
 */
void readResponse(int sock_fd, char *buffer, int resp_len)
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
			now = time(NULL);
			//fprintf(logfile, "%s: Error (read): %s\n", strtok(ctime(&now), "\n"), error);
			return;
		}
		buffer += sock_read;
		resp_len -= sock_read;
		/* Break the loop when the request structure is read completely */
		if (!sock_read || resp_len <= 0)
			break;
	}
	cout<<sock_read<<" bytes read\n";
	return;
}

/*
 * parsePacket() - parse the packet and validate all fields
 * req: request structure
 * return 0(Valid Packet) -1(Invalid Packet)
 */
int parsePacket(struct packet *resp)
{
	DEBUG("Parsing Packet\n");
	/* TODO : Complete the function */
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
		now = time(NULL);
		//fprintf(logfile, "%s: Error (parsePacket): Invalid command, code = %d\n",
				//strtok(ctime(&now), "\n"), resp->cmd_code);
		return -1;
	}
	return 0;
}

/*
 * processResponse()
 * req: request structure
 * return 0(request processed successfully) -1(request processing failed)
 */
int processResponse(int sock_fd, struct packet *resp)
{

	if(resp->cmd_code == LIST || resp->cmd_code == SHOW || resp->cmd_code == NOTIFY)
		displayContents(resp);
	else if (resp->cmd_code == LOGIN)
    	sessionID = resp->sessionId;
	else
	{
		printf("Error (processResponse):Invalid Option\n");
		now = time(NULL);
		//fprintf(logfile, "%s: Error (processResponse): Invalid Option, command code = %d\n",
						//strtok(ctime(&now), "\n"), resp->cmd_code);
		return -1;
	}
	return 0;
}

void displayContents(struct packet *resp)
{
	cout<<resp->contents.rvcd_cnts<<endl;
	return;
}


