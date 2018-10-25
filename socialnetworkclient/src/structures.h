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

struct request_packet
{
	unsigned int content_len;
	unsigned short int userID;
	char sessionID[SESSIONID_LEN];	
	char command[COMMAND_LEN];	
	int request_number;
	wchar_t content;
};

struct response_packet
{
	unsigned int content_len;
	char command[COMMAND_LEN];
	int request_number;
	wchar_t content;
};
	
	

