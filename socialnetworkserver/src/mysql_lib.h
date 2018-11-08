/*
 * mysql_lib.h
 *
 *  Created on: Oct 25, 2018
 *      Author: michael
 */

#ifndef MYSQL_LIB_H_
#define MYSQL_LIB_H_

#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

void test(void) {

	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;

		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root", "^AVdoDz%9WLEbHKQG7Ed");

	} catch (sql::SQLException &e) {
		std::cout << "errors\n";
	}
	exit(EXIT_SUCCESS);
}

#endif /* MYSQL_LIB_H_ */
