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

using namespace std;
/* Function Declarations */

/* server.cpp */
int serverInit(string port);
int acceptConnections(int master_fd);
void terminateClient(int slave_fd);

/* processClient.cpp */
void handleClient(int sock_fd);
void readRequest(int sock_fd, char *buffer, int req_len);
int parsePacket(struct request *req);
int sessionValidity(struct request *req);

/* processRequests.cpp */
int processRequest(struct request *req);
void userLogin(struct request req);
void userLogout(struct request req);
void listAllUsers(struct request req);
void postMessage(struct request req);

#endif /* FUNC_LIB_H_ */
