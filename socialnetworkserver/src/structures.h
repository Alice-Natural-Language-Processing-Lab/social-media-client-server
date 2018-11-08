/*
 * structures.h
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

#include <unordered_map>
using namespace std;

#define CMD_LEN			100

enum commands {
	LOGIN,
	LOGOUT,
	POST,
	SHOW,
	LIST
};
/**
 * packet - structure to store the message
 * command: to store the command portion of the request
 */
struct packet
{
	unsigned int content_len;
	enum commands cmd_code;
	unsigned int req_num;
	unordered_map<string, string> contents;
};




#endif /* STRUCTURES_H_ */
