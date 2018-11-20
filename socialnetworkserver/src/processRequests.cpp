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

using namespace std;
#define DEBUG printf

int processRequest(int sock_fd, struct packet *req);
void userLogin(int sock_fd, struct packet req);
void userLogout(int sock_fd, struct packet req);
void listAllUsers(int sock_fd, struct packet req);
void postMessage(int sock_fd, struct packet req);
void showWallMessage(int sock_fd, struct packet req);
int sendPacket(int sock_fd, struct packet *req, string value1);
int sendPacket(int sock_fd, struct packet *req, unsigned int value1);
int sendPacket(int sock_fd, struct packet &resp);

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
	sessionID = 101;

	/* TODO: User login to DB
	 * generate session id*/
	DEBUG("user name is :");
	cout<<req.contents.username<<endl;
	DEBUG("password is :");
	cout<<req.contents.password<<endl;

	ret = database.login(req);
	if (ret < 0)
	{
		printf("Error (login): Login to database failed\n");
		return;
	}
	sendPacket(sock_fd, &req, sessionID);
	/*
	pthread_mutex_lock(&notify_mutex);
	pthread_cond_signal(&notify_cond);
	pthread_mutex_unlock(&notify_mutex);
	*/
	return;
}

/*
 * userLogout() - logout request for user
 * req: request structure
 */
void userLogout(int sock_fd, struct packet req)
{
	int ret;

	/* TODO: User logout from DB*/
	DEBUG("Terminating client connection\n");
	ret = database.logout(req);
	if (ret < 0)
	{
		printf("Error (logout): User logging out from database failed\n");
		return;
	}
	return;
}

/*
 * listAllUsers() - List all users in the DB
 * req: request structure
 */
void listAllUsers(int sock_fd, struct packet req)
{
	int ret;

	/* TODO: List all users from DB*/
	//string userlist = "a,b,c,d,e,f";
	ret = database.listUsers(req);
	if (ret < 0)
	{
		printf("Error (listUsers): Fetching user list from database failed\n");
		return;
	}
	sendPacket(sock_fd, req);
	return;
}

/*
 * postMessage() - Post a message to a user's wall
 * req: request structure
 */
void postMessage(int sock_fd, struct packet req)
{
	int ret;

	/* TODO: Post to a wall */
	DEBUG("postee is %s\n", req.contents.postee);
	DEBUG("post is %s\n", req.contents.post);
	ret = database.postOnWall(req);
	if (ret < 0)
	{
		printf("Error (postOnWall): post to database wall failed\n");
		return;
	}
	/*
	pthread_mutex_lock(&notify_mutex);
	pthread_cond_signal(&notify_cond);
	pthread_mutex_unlock(&notify_mutex);
	*/

	return;
}

void showWallMessage(int sock_fd, struct packet req)
{
	int ret;

	/* TODO: send the wall */
	DEBUG("show %s's wall\n", req.contents.wallOwner);
	//string wall = "hi \nhello \nhow are you";
	ret = database.showWall(req);
	if (ret < 0)
	{
		printf("Error (showWall): Fetching wall from database failed\n");
		return;
	}
	sendPacket(sock_fd, req);
	return;
}

int sendPacket(int sock_fd, struct packet *req, string value1)
{
    struct packet resp;
    int send_bytes;

    resp.cmd_code = req->cmd_code;
    resp.sessionId = req->sessionId;
    switch(resp.cmd_code)
    {
    case LIST:
    	strcpy(resp.contents.rvcd_cnts, value1.c_str());
    	//pkt.contents.rvcd_cnts = value1;
    	break;
    case SHOW:
    	strcpy(resp.contents.rvcd_cnts, value1.c_str());
    	//pkt.contents.rvcd_cnts = value1;
    	break;
    default:
    	printf("Invalid Command Code\n");
    	return -1;
    }
    resp.req_num = req->req_num;
    resp.content_len = strlen(resp.contents.rvcd_cnts);

    send_bytes = write(sock_fd, &resp, sizeof(resp));
    if (send_bytes < 0)
    {
        perror("Write error\n");
        return -1;
    }
    cout<<send_bytes<<" bytes sent\n";
    return 0;
}

int sendPacket(int sock_fd, struct packet *req, unsigned int value1)
{
    struct packet resp;
    int send_bytes;

    resp.cmd_code = req->cmd_code;
    switch(resp.cmd_code)
    {
    case LOGIN:
    	resp.sessionId = value1;
    	break;
    default:
    	printf("Invalid Command Code\n");
    	return -1;
    }
    resp.req_num = req->req_num;
    resp.content_len = strlen(resp.contents.rvcd_cnts);

    send_bytes = write(sock_fd, &resp, sizeof(resp));
    if (send_bytes < 0)
    {
        perror("Write error\n");
        return -1;
    }
    cout<<send_bytes<<" bytes sent\n";
    return 0;
}

int sendPacket(int sock_fd, struct packet &resp)
{
	int send_bytes;

	send_bytes = write(sock_fd, &resp, sizeof(resp));
	if (send_bytes < 0)
	{
		perror("Write error\n");
		return -1;
	}
	cout<<send_bytes<<" bytes sent\n";
	return 0;

}
