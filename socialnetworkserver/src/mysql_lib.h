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
#include <string>

class MySQLDatabase {
private:
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::ResultSet *res;
	sql::ResultSetMetaData *result_set_meta_data;
	unsigned int column_count;
public:
	std::string query;
	MySQLDatabase();
	~MySQLDatabase();
	void getResults(void);
};

MySQLDatabase::MySQLDatabase() {
	try {
		// Create a connection
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root",
				"socialnetworkpswd");
		con->setSchema("SocialNetwork");
	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

MySQLDatabase::~MySQLDatabase() {
	delete con;
	delete stmt;
	delete res;
}

void MySQLDatabase::getResults(void) {
	try {
		if (query.empty()) {
			std::cout << "Query empty. Set MySQLDatabase.query\n";
		} else {
			stmt = con->createStatement();
			res = stmt->executeQuery(query.c_str());
			result_set_meta_data = res->getMetaData();
			column_count = result_set_meta_data->getColumnCount();
			std::cout << "\nResults:\n";
			while (res->next()) {

				for (unsigned int i = 1; i <= column_count; i++) {
					if (res->isFirst())
						std::cout << "\t"
								<< result_set_meta_data->getColumnName(i);
					else
						std::cout << "\t" << res->getString(i);
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}

	catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

/*
void test_query(std::string query) {
	try {
		sql::Driver *driver;
		sql::Connection *con;
		sql::Statement *stmt;
		sql::ResultSet *res;
		sql::ResultSetMetaData *result_set_meta_data;
		unsigned int column_count;

// Create a connection
		driver = get_driver_instance();
		con = driver->connect("tcp://127.0.0.1:3306", "root",
				"socialnetworkpswd");
		con->setSchema("SocialNetwork");

		stmt = con->createStatement();
		res = stmt->executeQuery(query.c_str());
		result_set_meta_data = res->getMetaData();
		column_count = result_set_meta_data->getColumnCount();
		std::cout << "\nResults:\n";
		while (res->next()) {

			for (unsigned int i = 1; i <= column_count; i++) {
				if (res->isFirst())
					std::cout << "\t" << result_set_meta_data->getColumnName(i);
				else
					std::cout << "\t" << res->getString(i);
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		delete res;
		delete stmt;
		delete con;

	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}
*/

#endif /* MYSQL_LIB_H_ */
