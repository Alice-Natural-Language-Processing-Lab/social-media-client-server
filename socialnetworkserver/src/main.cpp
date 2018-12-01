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

void test_hasValidSession(DatabaseCommandInterface& commandInterface);
void test_getResults(DatabaseCommandInterface& commandInterface);
void test_login(DatabaseCommandInterface& commandInterface);
void test_listUsers(DatabaseCommandInterface& commandInterface);
void test_showWall(DatabaseCommandInterface& commandInterface);
void test_postOnWall(DatabaseCommandInterface& commandInterface);
void test_logout(DatabaseCommandInterface& commandInterface);
void test_notifications(DatabaseNotificationInterface& notificationInterface);

packet test_packet1, test_packet2, test_packet3;

int main() {

	MySQLDatabaseDriver databaseDriver;
	DatabaseCommandInterface commandInterface(databaseDriver, SERVER_URL,
	SERVER_USERNAME,
	SERVER_PASSWORD, SERVER_DATABASE);
	{
		DatabaseNotificationInterface notificationInterface(databaseDriver,
		SERVER_URL,
		SERVER_USERNAME,
		SERVER_PASSWORD, SERVER_DATABASE);

		//while (true){
		//test_getResults(database);
		test_login(commandInterface);
		//test_listUsers(database);
		test_postOnWall(commandInterface);
		test_showWall(commandInterface);
		//test_logout(database);
		test_hasValidSession(commandInterface);
		test_notifications(notificationInterface);
		//getchar();
	}

	exit(0);
}

void test_hasValidSession(DatabaseCommandInterface& commandInterface) {

	//string timeout;
	//test_packet1.sessionId = 3456789012;
	//test_packet2.sessionId = 2345678901;

	//getline(cin, timeout);
	//commandInterface.session_timeout = atoi(timeout.c_str());

	cout << "test packet1:" << commandInterface.hasValidSession(test_packet1)
			<< "\n" << test_packet1.contents.rcvd_cnts << endl;
	cout << "test packet2:" << commandInterface.hasValidSession(test_packet2)
			<< "\n" << test_packet2.contents.rcvd_cnts << endl;
}

void test_getResults(DatabaseCommandInterface& commandInterface) {

	string query;

	getline(cin, query);
	commandInterface.getResults(query);
}

void test_login(DatabaseCommandInterface& commandInterface) {

	unsigned int socket_descriptor_test = 5;
	test_packet1.contents.username = "alex";
	test_packet1.contents.password = "17663506432727786073";
	test_packet2.contents.username = "ben";
	test_packet2.contents.password = "12927111708687947557";
	test_packet3.contents.username = "cris";
	test_packet3.contents.password = "11740314204215096121";

	commandInterface.login(test_packet1, socket_descriptor_test);
	commandInterface.login(test_packet2, socket_descriptor_test);
	commandInterface.login(test_packet3, socket_descriptor_test);
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

void test_listUsers(DatabaseCommandInterface& commandInterface) {

	commandInterface.listUsers(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;
}

void test_showWall(DatabaseCommandInterface& commandInterface) {

	test_packet1.contents.wallOwner = "alex";

	commandInterface.showWall(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	test_packet1.contents.wallOwner = "cris";

	commandInterface.showWall(test_packet1);
	cout << "test_packet2:\n" << test_packet1.contents.rcvd_cnts << endl;

	test_packet1.contents.wallOwner = "ben";

	commandInterface.showWall(test_packet1);
	cout << "test_packet3:\n" << test_packet1.contents.rcvd_cnts << endl;
}

void test_postOnWall(DatabaseCommandInterface& commandInterface) {

	test_packet1.contents.postee = "cris";
	test_packet1.contents.post = "Hey! Wanna eat some ice cream!";

	commandInterface.postOnWall(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	test_packet2.contents.postee = "ben";
	test_packet2.contents.post = "Hey! Wanna eat some ice cream!";

	commandInterface.postOnWall(test_packet2);
	cout << "test_packet1:\n" << test_packet2.contents.rcvd_cnts << endl;
}

void test_logout(DatabaseCommandInterface& commandInterface) {

	commandInterface.logout(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	commandInterface.logout(test_packet1);
	cout << "test_packet1:\n" << test_packet1.contents.rcvd_cnts << endl;

	commandInterface.logout(test_packet2);
	cout << "test_packet2:\n" << test_packet2.contents.rcvd_cnts << endl;
}

void test_notifications(DatabaseNotificationInterface& notificationInterface) {

	cout << "notification interface:\n"
			<< notificationInterface.getNotifications() << endl;
	int i = 1;
	//while (i > 0)
	{
		i = notificationInterface.next();
		cout << i << endl;
		cout << "socket: "
				<< notificationInterface.sendNotification(test_packet1) << endl
				<< test_packet1.contents.rcvd_cnts << endl;
		cout << "read: " << notificationInterface.markRead() << endl;
	}
}
