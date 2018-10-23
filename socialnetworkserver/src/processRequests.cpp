/*
 * processRequests.cpp
 *
 *  Created on: Oct 22, 2018
 *      Author: pournami
 */
#include "func_lib.h"

#define DEBUG printf

int processRequest(struct request *req);
void userLogin(struct request *req);
void userLogout(struct request *req);
void listAllUsers(struct request *req);
void postMessage(struct request *req);

/*
 * processRequest() - validate the client session
 * req: request structure
 * return 0(request processed successfully) -1(request processing failed)
 */
int processRequest(struct request *req)
{
	if (!strcmp(req->command, "login"))
		userLogin(req);
	else if(!strcmp(req->command, "logout"))
		userLogout(req);
	else if(!strcmp(req->command, "list"))
		listAllUsers(req);
	else if(!strcmp(req->command, "post"))
		postMessage(req);
	else
		printf("Invalid Option\n");
	return 0;
}

/*
 * userLogin() - login request for user
 * req: request structure
 */
void userLogin(struct request *req)
{
	DEBUG("User Login Request\n");
	/* TODO: User login to DB*/
	return;
}

/*
 * userLogout() - logout request for user
 * req: request structure
 */
void userLogout(struct request *req)
{
	DEBUG("User Logout Request\n");
	/* TODO: User logout from DB*/
	return;
}

/*
 * listAllUsers() - List all users in the DB
 * req: request structure
 */
void listAllUsers(struct request *req)
{
	DEBUG("Request to list all users\n");
	/* TODO: List all users*/
	return;
}

/*
 * postMessage() - Post a message to a user's wall
 * req: request structure
 */
void postMessage(struct request *req)
{
	DEBUG("Request to post to wall\n");
	/* TODO: Post to a wall */
	return;
}
