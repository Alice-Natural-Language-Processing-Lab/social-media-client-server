/*
 * readProcess.cpp
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
#include "structures.h"
#include "func_lib.h"
extern string username;
extern const char * getCommand(int enumVal);
extern FILE *logfile;
extern time_t now;
extern char *error;
extern unsigned int sessionID;

using namespace std;

void readThread(int sock_fd);
int sendPacket(int sock_fd, enum commands cmd_code, string key, string value);
void listUsers(int sock_fd);
void printCmdList();
void post(int sock_fd);
void show(int sock_fd);
void logout(int sock_fd);
void createLoginPacket(string username, string pw, struct packet &pkt);
void createPostPacket(string postee, string post, struct packet &pkt);
void createShowPacket(string wallOwner, struct packet &pkt);

#define DEBUG	printf
#define ERR_LEN 256

int logoutSignal;
int req_num;

void readThread(int sock_fd)
{
    /*Print the list of commands*/
	DEBUG("Inside read thread\n");
    int cmd_entered;
    printCmdList();
    while(1)
    {
        cin>>cmd_entered;
        if (cmd_entered == 0)
            printCmdList();
        else if (cmd_entered == 1)
        {
            /*Call list users function*/
            listUsers(sock_fd);
        }
        else if (cmd_entered == 2)
        {
            /*Call post function*/
            post(sock_fd);
        }
        else if (cmd_entered == 3)
        {
        	show(sock_fd);
        }
        else if (cmd_entered == 4)
        {
        	logout(sock_fd);
        	close(sock_fd);
        	exit(0);
        }
        else
        {
            cout<<"Invalid Option. Try again !!!\n";
        }
    }
    pthread_exit(NULL);
}

void listUsers(int sock_fd)
{
    sendPacket(sock_fd, LIST, "", "");
    return;
}

void post(int sock_fd)
{
    int postWall;
    string name;
    string post;

    cout<<"1. Own wall\n";
    cout<<"2. Others wall\n";
    cin>>postWall;
	if (postWall == 1)
	{
		name = username;
	}
	else if (postWall == 2)
	{
		cout<<"Enter postee name: ";
		cin>>name;
	}
	else
	{
		cout<<"Invalid Option\n";
		return;
	}
	cout<<"Enter Msg: ";
	cin.ignore();
	getline(std::cin, post);
	sendPacket(sock_fd, POST, name, post);
    return;
}

void show(int sock_fd)
{
    int showWall;
    string name;

    cout<<"1. Own wall\n";
    cout<<"2. Others wall\n";
    cin>>showWall;
	if (showWall == 1)
	{
		name = username;
	}
	else if (showWall == 2)
	{
		cout<<"Whose wall: ";
		cin>>name;
	}
	else
	{
		cout<<"Invalid Option\n";
		return;
	}
	sendPacket(sock_fd, SHOW, name, "");
    return;
}

void logout(int sock_fd)
{
	sendPacket(sock_fd, LOGOUT, "", "");
}

void printCmdList()
{
    cout<<"Commands (Enter 0- 3)\n"<<"--------------\n";
    cout<<"1. List all users\n";
    cout<<"2. Post to wall\n";
    cout<<"3. Show wall\n";
    cout<<"4. Logout\n";
    cout<<"[Enter 0 to print the command list]\n\n";
    return;
}


int sendPacket(int sock_fd, enum commands cmd_code, string value1, string value2)
{
    struct packet req;
    int send_bytes;

    req.cmd_code = cmd_code;
    req.req_num = ++req_num;
    req.sessionId = sessionID;
    switch(cmd_code)
    {
    case LOGIN:
    	createLoginPacket(value1, value2, req);
    	break;
    case LOGOUT:
    	break;
    case LIST:
    	break;
    case POST:
    	createPostPacket(value1, value2, req);
    	break;
    case SHOW:
    	createShowPacket(value1, req);
    	break;
    default:
    	printf("Invalid Command Code\n");
    	return -1;
    }
    req.content_len = strlen(req.contents.username) + strlen(req.contents.password) + strlen(req.contents.postee) +
    					strlen(req.contents.post) + strlen(req.contents.wallOwner);
    send_bytes = write(sock_fd, &req, sizeof(req));
    now = time(NULL);
    if (send_bytes < 0)
    {
    	strerror_r(errno, error, ERR_LEN);
    	printf("Error (write): %s\n", error);
    	fprintf(logfile, "%s: Error (read): %s\n", strtok(ctime(&now), "\n"), error);
        return -1;
    }
    fprintf(logfile, "%s: Request sent:\n", strtok(ctime(&now), "\n"));
    fprintf(logfile, "%d | %s | %d | %d\n", req.content_len, getCommand(req.cmd_code), req.req_num, req.sessionId);
    DEBUG("%d bytes sent\n", send_bytes);
    return 0;
}

void createLoginPacket(string username, string pw, struct packet &pkt)
{
	strcpy(pkt.contents.username, (const char *)username.c_str());
	strcpy(pkt.contents.password, (const char *)pw.c_str());
	//pkt.contents.username = username;
	//pkt.contents.password = pw;

}

void createPostPacket(string postee, string post, struct packet &pkt)
{
	strcpy(pkt.contents.postee, (const char *)postee.c_str());
	strcpy(pkt.contents.post, (const char *)post.c_str());
	//pkt.contents.postee = postee;
	//pkt.contents.post = post;
}

void createShowPacket(string wallOwner, struct packet &pkt)
{
	strcpy(pkt.contents.wallOwner, (const char *)wallOwner.c_str());
	//pkt.contents.wallOwner = wallOwner;
}


