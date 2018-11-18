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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

/*
returns positive int socketfd when success
return -1 if failed to create socket
return -2 if failed to bind
return -3 if failed to listen
*/
int create_socket(bool server);

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


int write(int socketfd, struct packet &pkt);
int read(int socketfd, struct packet &pkt);

#endif /* NETWORKING_H_ */
