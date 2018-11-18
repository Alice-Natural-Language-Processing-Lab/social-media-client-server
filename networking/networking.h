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

int create_socket(bool server);
int destroy_socket(int socketfd);
int write(int socketfd, struct packet &pkt);
int read(int socketfd, struct packet &pkt);

#endif /* NETWORKING_H_ */
