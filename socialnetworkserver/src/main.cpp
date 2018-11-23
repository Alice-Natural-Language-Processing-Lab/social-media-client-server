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

	string query, user_name;
	MySQLDatabaseDriver databaseDriver;
	MySQLDatabaseInterface database(&databaseDriver, SERVER_URL, SERVER_USERNAME,
	SERVER_PASSWORD, SERVER_DATABASE);
	Notifications notifications(&database);

	while (true) {
		getline(cin, query);
		database.getResults(query);
	}

	exit(EXIT_SUCCESS);
}
