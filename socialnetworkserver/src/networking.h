/*
 * networking.h
 *
 *  Created on: Nov 8, 2018
 *      Author: pan
 */

#ifndef NETWORKING_H_
#define NETWORKING_H_

#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include "structures.h"

#define TIMEOUT_SEC 5
#define LISTEN_QUEUE_LENGTH 15
#define MAX_PACKET_LEN 4096
#define ERR_LEN 256

using namespace std;

/*
return positive int socketfd when success
return -1 if failed to create socket
return -2 if failed to bind
return -3 if failed to listen
return -4 if failed to set REUSE ADDR or PORT
*/
int create_server_socket(int portNum);

/*
return positive int socketfd when success
return -1 if getaddrinfo() failed
return -2 if create socket failed
return -3 if failed to set REUSE ADDR or PORT
return -4 if no socket succeeded to connect
*/
int create_client_socket(string serverName, int portNum);

/*
returns positive int slaveSocketfd when success
return -1 if failed to accept
*/
int accept_socket(int socketfd);

/*
return 0 if close socket success
return -1 if failed to close
*/
int destroy_socket(int socketfd);

/*
send the pkt through the socket,
this function will automatically set content_len & req_num field for the packet
return 0 if success
return -1 if error happened in the write() funciton
*/
int write_socket(int socketfd, struct packet &pkt);

/*
read the message from the socket, and set field for the pkt
return 0 if read message is empty
return -1 if recieved packet format is wrong
return -2 if error happened in the read() funciton
return positive number if success, return is the total bytes of the message
*/
int read_socket(int socketfd, struct packet &pkt);

#endif /* NETWORKING_H_ */
