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

void test_hasValidSession(DatabaseCommandInterface& database);
void test_getResults(DatabaseCommandInterface& database);
void test_login(DatabaseCommandInterface& database);
void test_listUsers(DatabaseCommandInterface& database);
void test_showWall(DatabaseCommandInterface& database);
void test_postOnWall(DatabaseCommandInterface& database);
void test_logout(DatabaseCommandInterface& database);

packet test_packet1, test_packet2, test_packet3;

int main() {

	MySQLDatabaseDriver databaseDriver;
	DatabaseCommandInterface database(databaseDriver, SERVER_URL,
	SERVER_USERNAME,
	SERVER_PASSWORD, SERVER_DATABASE);
	DatabaseNotificationInterface notifications(&database);

	while (true) {
		//test_getResults(database);
		test_login(database);
		//test_listUsers(database);
		test_postOnWall(database);
		test_showWall(database);
		//test_logout(database);
		test_hasValidSession(database);
		getchar();
	}

	exit(0);
}

void test_hasValidSession(DatabaseCommandInterface& database) {

	//string timeout;
	//test_packet1.sessionId = 3456789012;
	//test_packet2.sessionId = 2345678901;

	//getline(cin, timeout);
	//database.session_timeout = atoi(timeout.c_str());

	cout << "test packet1:" << database.hasValidSession(test_packet1) << "\n"
			<< test_packet1.contents.rcvd_cnts << endl;
	cout << "test packet2:" << database.hasValidSession(test_packet2) << "\n"
			<< test_packet2.contents.rcvd_cnts << endl;
}

void test_getResults(DatabaseCommandInterface& database) {

	string query;

	getline(cin, query);
	database.getResults(query);
}

void test_login(DatabaseCommandInterface& database) {

	unsigned int socket_descriptor_test = 5;
	test_packet1.contents.username = "alex";
	test_packet1.contents.password = "17663506432727786073";
	test_packet2.contents.username = "ben";
	test_packet2.contents.password = "12927111708687947557";
	test_packet3.contents.username = "cris";
	test_packet3.contents.password = "11740314204215096121";

	database.login(test_packet1, socket_descriptor_test);
	database.login(test_packet2, socket_descriptor_test);
	database.login(test_packet3, socket_descriptor_test);
	cout << "test_packet1:" << endl << "sessionid: " << test_packet1.sessionId
			<< endl << "rcvd_contents: " << test_packet1.contents.rcvd_cnts
			<< endl;
	cout << "test_packet2:" << endl << "sessionid: " << test_packet2.sessionId
			<< endl << "rcvd_contents: " << test_packet2.contents.rcvd_cnts
			<< endl;
	cout << "test_packet3:" << endl << "sessionid: " << test_packet3.sessionId
			<< endl << "rcvd_contents: " << test_packet3.contents.rcvd_cnts
			<< endl;
}

void test_listUsers(DatabaseCommandInterface& database) {

	database.listUsers(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;
}

void test_showWall(DatabaseCommandInterface& database) {

	test_packet1.contents.wallOwner = "alex";

	database.showWall(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	test_packet1.contents.wallOwner = "cris";

	database.showWall(test_packet1);
	cout << "test_packet2:\n" << test_packet1.contents.rcvd_cnts << endl;

	test_packet1.contents.wallOwner = "ben";

	database.showWall(test_packet1);
	cout << "test_packet3:\n" << test_packet1.contents.rcvd_cnts << endl;
}

void test_postOnWall(DatabaseCommandInterface& database) {

	test_packet1.contents.postee = "cris";
	test_packet1.contents.post = "Hey! Wanna eat some ice cream!";

	database.postOnWall(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	test_packet2.contents.postee = "ben";
	test_packet2.contents.post = "Hey! Wanna eat some ice cream!";

	database.postOnWall(test_packet2);
	cout << "test_packet1:\n" << test_packet2.contents.rcvd_cnts << endl;
}

void test_logout(DatabaseCommandInterface& database) {

	database.logout(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	database.logout(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	database.logout(test_packet2);
	cout << "test_packet2:\n" << test_packet2.contents.rcvd_cnts << endl;
}
