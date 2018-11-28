/*
 * processRequests.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: pournami
 */
#include "func_lib.h"
#include "structures.h"
#include  "mysql_lib.h"
extern MySQLDatabaseInterface database;

extern pthread_cond_t notify_cond;
extern pthread_mutex_t notify_mutex;
extern int notify_variable;

using namespace std;
#define DEBUG

unsigned int sessionID;

/*
 * processRequest() - validate the client session
 * req: request structure
 * return 0(request processed successfully) -1(request processing failed)
 */
int processRequest(int sock_fd, struct packet *req)
{
	struct packet pkt;

	memset(&pkt, 0, sizeof(pkt));
	memcpy(&pkt, req, sizeof(pkt));

	if (req->cmd_code == LOGIN)
		userLogin(sock_fd, pkt);
	else if(req->cmd_code == LOGOUT)
		userLogout(sock_fd, pkt);
	else if(req->cmd_code == LIST)
		listAllUsers(sock_fd, pkt);
	else if(req->cmd_code == POST)
		postMessage(sock_fd, pkt);
	else if(req->cmd_code == SHOW)
		showWallMessage(sock_fd, pkt);
	else
		printf("Invalid Option\n");
	return 0;
}

/*
 * userLogin() - login request for user
 * req: request structure
 */
void userLogin(int sock_fd, struct packet req)
{
	int ret;

	ret = database.login(req, sock_fd);
	sendPacket(sock_fd, req);
	if (ret == -2)
	{
		printf("Error (login): DB login error\nClosing Client Connection\n");
		destroy_socket(sock_fd);
		pthread_exit(NULL);
		return;
	}
	/*
	pthread_mutex_lock(&notify_mutex);
	notify_variable = 1;
	pthread_cond_signal(&notify_cond);
	pthread_mutex_unlock(&notify_mutex);
	*/
	return;
}

/*
 * listAllUsers() - List all users in the DB
 * req: request structure
 */
void listAllUsers(int sock_fd, struct packet req)
{
	int ret;

	ret = database.listUsers(req);
	sendPacket(sock_fd, req);
	if (ret == -2)
	{
		printf("Error (listUsers): DB listUsers error\nClosing Client Connection");
		destroy_socket(sock_fd);
		pthread_exit(NULL);
		return;
	}
	return;
}

/*
 * postMessage() - Post a message to a user's wall
 * req: request structure
 */
void postMessage(int sock_fd, struct packet req)
{
	int ret;

	DEBUG("postee is %s\n", req.contents.postee.c_str());
	DEBUG("post is %s\n", req.contents.post.c_str());
	ret = database.postOnWall(req);
	if (ret < 0)
	{
		printf("Error (postOnWall): post to database wall failed\n");
		sendPacket(sock_fd, req);
		return;
	}
	/*
	pthread_mutex_lock(&notify_mutex);
	notify_variable = 1;
	pthread_cond_signal(&notify_cond);
	pthread_mutex_unlock(&notify_mutex);
	*/
	return;
}

/*
 * showWallMessage() - show a user's wall
 * req: request structure
 */
void showWallMessage(int sock_fd, struct packet req)
{
	int ret;

	DEBUG("show %s's wall\n", req.contents.wallOwner.c_str());
	ret = database.showWall(req);
	sendPacket(sock_fd, req);
	if (ret == -2)
	{
		printf("Error (showWall): DB show Wall error\nClosing Client Connection\n");
		destroy_socket(sock_fd);
		pthread_exit(NULL);
		return;
	}
	return;
}

/*
 * userLogout() - logout request for user
 * req: request structure
 */
void userLogout(int sock_fd, struct packet req)
{
	int ret;

	DEBUG("Terminating client connection\n");
	ret = database.logout(req);
	if (ret < 0)
	{
		printf("Error (logout): User logging out from database failed\n");
		return;
	}
	destroy_socket(sock_fd);
	pthread_exit(NULL);
	return;
}

/*
 * sendPacket() - send the response to client
 * resp: response packet
 * returns 0 if success -1 if error
 */
int sendPacket(int sock_fd, struct packet &resp)
{
	int send_bytes;

	send_bytes = write_socket(sock_fd, resp);
	if (send_bytes < 0)
	{
		printf("Error (write_socket)\n");
		return -1;
	}
	DEBUG("%d bytes sent\n", send_bytes);
	return 0;
}
