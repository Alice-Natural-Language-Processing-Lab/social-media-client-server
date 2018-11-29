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
#include <iostream>
#include <tr1/functional>

#include "mysql_lib.h"

#define SERVER_URL "tcp://127.0.0.1:3306"
#define SERVER_USERNAME "root"
#define SERVER_PASSWORD "socialnetworkpswd"
#define SERVER_DATABASE "SocialNetwork"

using namespace std;

void test_hasValidSession(MySQLDatabaseInterface& database);
void test_getResults(MySQLDatabaseInterface& database);
void test_login(MySQLDatabaseInterface& database);
void test_listUsers(MySQLDatabaseInterface& database);
void test_showWall(MySQLDatabaseInterface& database);
size_t hash_func(string input);

int main() {

	MySQLDatabaseDriver databaseDriver;
	MySQLDatabaseInterface database(&databaseDriver, SERVER_URL,
	SERVER_USERNAME,
	SERVER_PASSWORD, SERVER_DATABASE);
	Notifications notifications(&database);

	string input;

	while (true) {
		//test_getResults(database);
		//test_hasValidSession(database);
		test_login(database);
		//test_listUsers(database);
		//test_showWall(database);
		getchar();
		//getline(cin, input);
		//cout << hash_func(input);
	}

	exit(0);
}

void test_hasValidSession(MySQLDatabaseInterface& database) {

	string timeout;
	packet test_packet1, test_packet2;
	test_packet1.sessionId = 3456789012;
	test_packet2.sessionId = 2345678901;

	getline(cin, timeout);
	database.session_timeout = atoi(timeout.c_str());

	cout << "test packet1:" << database.hasValidSession(test_packet1) << "\n"
			<< test_packet1.contents.rcvd_cnts << endl;
	cout << "test packet2:" << database.hasValidSession(test_packet2) << "\n"
			<< test_packet2.contents.rcvd_cnts << endl;
}

void test_getResults(MySQLDatabaseInterface& database) {

	string query;

	getline(cin, query);
	database.getResults(query);
}

void test_login(MySQLDatabaseInterface& database) {

	unsigned int socket_descriptor_test = 5;
	packet test_packet1, test_packet2, test_packet3;
	test_packet1.contents.username = "alex";
	test_packet1.contents.password = "17663506432727786073";
	test_packet2.contents.username = "sam";
	test_packet2.contents.password = "wrongpass";
	test_packet3.contents.username = "nousername";
	test_packet3.contents.password = "doesntmatter";

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

void test_listUsers(MySQLDatabaseInterface& database) {

	packet test_packet1;

	database.listUsers(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;
}

void test_showWall(MySQLDatabaseInterface& database) {

	packet test_packet1, test_packet2;
	test_packet1.contents.wallOwner = "sam";
	test_packet2.contents.wallOwner = "sauron";

	database.showWall(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	database.showWall(test_packet2);
	cout << "test_packet2:\n" << test_packet2.contents.rcvd_cnts << endl;
}

size_t hash_func(string input) {

	std::tr1::hash<std::string> str_hash;
	return str_hash(input);
}
