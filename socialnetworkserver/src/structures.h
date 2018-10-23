/*
 * structures.h
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#define CMD_LEN			100

/**
 * request - structure to store the request message
 * command: to store the command portion of the request
 */
struct request
{
	char command[CMD_LEN];
};

/**
 * response - to store the response message
 */
struct response
{

};



#endif /* STRUCTURES_H_ */
