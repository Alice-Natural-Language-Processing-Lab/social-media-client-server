/*
 * main.cpp
 *
 *  Created on: Nov 7, 2018
 *      Author: michael
 */

#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <string>

#include "mysql_lib.h"

#define SERVER_URL "tcp://127.0.0.1:3306"
#define SERVER_USERNAME "root"
#define SERVER_PASSWORD "socialnetworkpswd"
#define SERVER_DATABASE "SocialNetwork"

using namespace std;

int main() {

	string query, user_name, timeout;
	MySQLDatabaseDriver databaseDriver;
	MySQLDatabaseInterface database(&databaseDriver, SERVER_URL,
	SERVER_USERNAME,
	SERVER_PASSWORD, SERVER_DATABASE);
	Notifications notifications(&database);

	packet test_packet1, test_packet2;
	test_packet1.sessionId = 3456789012;
	test_packet2.sessionId = 2345678901;

	getline(cin, timeout);
	database.session_timeout = atoi(timeout.c_str());

	cout << "test packet1:" << database.hasValidSession(test_packet1) << "\n"
			<< test_packet1.contents.rvcd_cnts << endl;
	cout << "test packet2:" << database.hasValidSession(test_packet2) << "\n"
			<< test_packet2.contents.rvcd_cnts << endl;

	while (true) {
		getline(cin, query);
		database.getResults(query);
	}

	exit(EXIT_SUCCESS);
}
