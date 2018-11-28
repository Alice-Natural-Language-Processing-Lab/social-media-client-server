/*
 * func_lib.h
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */

#ifndef FUNC_LIB_H_
#define FUNC_LIB_H_

#include <string>
#include <iostream>
#include "structures.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include "networking.h"

using namespace std;
/* Function Declarations */

static const char * commandList[] = { "LOGIN", "LOGOUT", "POST", "SHOW", "LIST", "NOTIFY", "ACK" };

/* server.cpp */
int serverInit(string port);
int acceptConnections(int master_fd);
void terminateClient(int slave_fd);

/* processClient.cpp */
void handleClient(int sock_fd);
int readRequest(int sock_fd, char *buffer, int req_len);
int parsePacket(struct packet *req);
int sessionValidity(struct packet *req);

/* processRequests.cpp */
int processRequest(int sock_fd, struct packet *req);
void userLogin(int sock_fd, struct packet req);
void userLogout(int sock_fd, struct packet req);
void listAllUsers(int sock_fd, struct packet req);
void postMessage(int sock_fd, struct packet req);
void showWallMessage(int sock_fd, struct packet req);
int sendPacket(int sock_fd, struct packet &resp);

/* processNotifications.cpp */
void processNotification();

#endif /* FUNC_LIB_H_ */
