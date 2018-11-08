/*
 * processRequests.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: pournami
 */
#include "func_lib.h"

#define DEBUG printf

int processRequest(struct packet *req);
void userLogin(struct packet *req);
void userLogout(struct packet *req);
void listAllUsers(struct packet *req);
void postMessage(struct packet *req);

/*
 * processRequest() - validate the client session
 * req: request structure
 * return 0(request processed successfully) -1(request processing failed)
 */
int processRequest(struct packet *req)
{
	if (req->cmd_code == LOGIN)
		userLogin(req);
	else if(req->cmd_code == LOGOUT)
		userLogout(req);
	else if(req->cmd_code == LIST)
		listAllUsers(req);
	else if(req->cmd_code == POST)
		postMessage(req);
	else
		printf("Invalid Option\n");
	return 0;
}

/*
 * userLogin() - login request for user
 * req: request structure
 */
void userLogin(struct packet *req)
{
	DEBUG("User Login Request\n");
	/* TODO: User login to DB*/
	return;
}

/*
 * userLogout() - logout request for user
 * req: request structure
 */
void userLogout(struct packet *req)
{
	DEBUG("User Logout Request\n");
	/* TODO: User logout from DB*/
	return;
}

/*
 * listAllUsers() - List all users in the DB
 * req: request structure
 */
void listAllUsers(struct packet *req)
{
	DEBUG("Request to list all users\n");
	/* TODO: List all users from DB*/
	return;
}

/*
 * postMessage() - Post a message to a user's wall
 * req: request structure
 */
void postMessage(struct packet *req)
{
	DEBUG("Request to post to wall\n");
	/* TODO: Post to a wall */
	return;
}
