/*
 * main.cpp
 *
 *  Created on: Nov 7, 2018
 *      Author: michael
 */

#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include "mysql_lib.h"
#include <string>

using namespace std;

void test(int &a){
	cout << a;
	a = 5;
}

int main() {

	string query, user_name;
	MySQLDatabase database;
	Credential credential;

	int a = 3;
	test(a);
	cout << a;

	getline(cin, query);
	database.getResults(query);
	getline(cin, user_name);
	credential = database.getCredential(user_name);
	cout << credential.userName << " | " << credential.passwordHash << " | "
			<< credential.salt << endl;

	exit(EXIT_SUCCESS);
}

