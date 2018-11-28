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

void test_hasValidSession(MySQLDatabaseInterface& database);
void test_getResults(MySQLDatabaseInterface& database);
void test_login(MySQLDatabaseInterface& database);

int main() {

	MySQLDatabaseDriver databaseDriver;
	MySQLDatabaseInterface database(&databaseDriver, SERVER_URL,
	SERVER_USERNAME,
	SERVER_PASSWORD, SERVER_DATABASE);
	Notifications notifications(&database);

	while (true) {
		//test_getResults(database);
		//test_hasValidSession(database);
		test_login(database);
		getchar();
	}

	exit(EXIT_SUCCESS);
}

void test_hasValidSession(MySQLDatabaseInterface& database) {

	string timeout;
	packet test_packet1, test_packet2;
	test_packet1.sessionId = 3456789012;
	test_packet2.sessionId = 2345678901;

	getline(cin, timeout);
	database.session_timeout = atoi(timeout.c_str());

	cout << "test packet1:" << database.hasValidSession(test_packet1) << "\n"
			<< test_packet1.contents.rvcd_cnts << endl;
	cout << "test packet2:" << database.hasValidSession(test_packet2) << "\n"
			<< test_packet2.contents.rvcd_cnts << endl;
}

void test_getResults(MySQLDatabaseInterface& database) {

	string query;

	getline(cin, query);
	database.getResults(query);
}

void test_login(MySQLDatabaseInterface& database) {

	int socket_descriptor_test = 5;
	packet test_packet1, test_packet2, test_packet3;
	test_packet1.contents.username = "frodo";
	test_packet1.contents.password = "123456789012345";
	test_packet2.contents.username = "sam";
	test_packet2.contents.password = "wrongpass";
	test_packet3.contents.username = "nousername";
	test_packet3.contents.password = "doesntmatter";
	//values ('frodo', '123456789012345'), ('sam', '543210987654321');
	cout << "test_packet1: " << database.login(test_packet1, socket_descriptor_test) << endl
			<< "sessionid: " << test_packet1.sessionId << endl
			<< "rcvd_contents: " << test_packet1.contents.rvcd_cnts << endl;
	cout << "test_packet2: " << database.login(test_packet2, socket_descriptor_test) << endl
			<< "sessionid: " << test_packet2.sessionId << endl
			<< "rcvd_contents: " << test_packet2.contents.rvcd_cnts << endl;
	cout << "test_packet3: " << database.login(test_packet3, socket_descriptor_test) << endl
			<< "sessionid: " << test_packet3.sessionId << endl
			<< "rcvd_contents: " << test_packet3.contents.rvcd_cnts << endl;
}
