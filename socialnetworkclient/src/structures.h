/*
 * structures.h
 *
 *  Created on: Oct 17, 2018
 *      Author: jagdeep
 */

#include<iostream>
using namespace std;

#define COMMAND_LEN 20
#define SESSIONID_LEN 20

struct packet
{
	int content_len;
	unsigned short int userID;
	wchar_t content;
	char command[COMMAND_LEN];
	char sessionID[SESSIONID_LEN];

};

