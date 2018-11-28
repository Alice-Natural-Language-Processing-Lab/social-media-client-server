/*
 * processClient.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */
#include <time.h>
#include "func_lib.h"
#include "mysql_lib.h"
#include "structures.h"

extern MySQLDatabaseInterface database;

using namespace std;

#define DEBUG

const char * getCommand(int enumVal)
{
  return commandList[enumVal];
}

/*
 * handleClient() - handle each client connection
 * sock_fd: slave socket file descriptor
 */
void handleClient(int sock_fd)
{
	int sock_read, ret;
	struct packet req;

	/* Accept the request persistently*/
	while(1)
	{
		memset(&req, 0, sizeof(struct packet));

		/* Read client request */
		sock_read = read_socket(sock_fd, req);
		if(sock_read < 0)
		{
			printf("Error(read_socket)\n");
			break;
		}
		if (!sock_read)
			break;

		/* Parse the packet for valid packet structure */
		ret = parsePacket(&req);
		if (ret < 0)
		{
			printf("Error (parsePacket): Packet parsing/checking failed\n");
			break;
		}
		DEBUG("Request received: ");
		DEBUG("%d | %s | %d | %u\n", req.content_len, getCommand(req.cmd_code), req.req_num, req.sessionId);

		/* Validate session of the client */
		ret = sessionValidity(&req);
		if (ret < 0)
		{
			printf("Error (sessionValidity): session validity could not be established\n");
			sendPacket(sock_fd, req);
			break;
		}

		/* process the request */
		ret = processRequest(sock_fd, &req);
		if (ret < 0)
		{
			printf("Error (processRequest): request processing failed\n");
			break;
		}
	}
	DEBUG("Client connection closed\n");
	destroy_socket(sock_fd);
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
	switch (req->cmd_code)
	{
	case LOGIN:
		break;
	case LOGOUT:
		break;
	case POST:
		break;
	case LIST:
		break;
	case SHOW:
		break;
	case ACK:
		break;
	case NOTIFY:
		break;
	default:
		printf("Invalid command, code = %d\n", req->cmd_code);
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
	int ret = 0;
	if (req->cmd_code != LOGIN)
		ret = database.hasValidSession(*req);
	return ret;
}

