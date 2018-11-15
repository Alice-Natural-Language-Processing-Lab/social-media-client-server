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

int main() {

	string query;
	MySQLDatabase database;

	while (true) {
		getline(cin, query);
		//test_query(query);
		database.query = query;
		database.getResults();
	}

	exit(EXIT_SUCCESS);
}
