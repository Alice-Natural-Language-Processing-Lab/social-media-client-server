/*
 * main.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: pournami
 */

#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <string>

#include "func_lib.h"
#include "mysql_lib.h"

using namespace std;

int main(int argc, char *argv[])
{
	string port = "5354";
	int master_fd, accept_conn;

	switch (argc)
	{
	case 1:
			break;
	case 2:
			port = argv[1];
			break;
	default:
			printf("Error: Usage is ./<executable> [port]\n");
			return -1;
	}
	master_fd = serverInit(port);
	if (master_fd < 0)
	{
		printf("Error (serverInit): Server Initialization failed\n");
		return -1;
	}
	accept_conn = acceptConnections(master_fd);
	if (accept_conn < 0)
	{
		printf("Error (acceptConnections): Connection accept error\n");
		return -1;
	}
	return 0;
}
