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
#include <string>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class Credential {

public:
	std::string userName;
	std::string passwordHash;
	std::string salt;
};

class MySQLDatabase {

private:
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::PreparedStatement *pstmt;
	sql::ResultSet *res;
	sql::ResultSetMetaData *result_set_meta_data;
	unsigned int column_count;

public:
	MySQLDatabase();
	~MySQLDatabase();
	void getResults(std::string query);
	Credential getCredential(std::string user);
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
}

void MySQLDatabase::getResults(std::string query) {

	try {
		stmt = con->createStatement();
		res = stmt->executeQuery(query.c_str());
		result_set_meta_data = res->getMetaData();
		column_count = result_set_meta_data->getColumnCount();
		std::cout << "\nResults:\n";
		do {
			for (unsigned int i = 1; i <= column_count; i++) {
				if (res->isBeforeFirst())
					std::cout << "\t" << result_set_meta_data->getColumnName(i);
				else
					std::cout << "\t" << res->getString(i);
			}
			std::cout << std::endl;
		} while (res->next());
		std::cout << std::endl;

		delete stmt;
		delete res;
	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

//void MySQLDatabase::listUsers(string *user_list, int *list_size)

Credential MySQLDatabase::getCredential(std::string user) {

	Credential cred;

	try {
		pstmt =
				con->prepareStatement(
						"select userName, passwordHash, salt from Users where userName = ?");
		pstmt->setString(1, user);
		res = pstmt->executeQuery();
		res->next();
		cred.userName = res->getString("userName");
		cred.passwordHash = res->getString("passwordHash");
		cred.salt = res->getString("salt");

		delete pstmt;
		delete res;
	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	return cred;
}

#endif /* MYSQL_LIB_H_ */
