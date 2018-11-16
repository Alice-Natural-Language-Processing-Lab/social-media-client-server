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

/*
enum function_returns {
	SUCCESS = 0, INVALID_SESSION_ID = -1, INVALID_CREDENTIALS = -2
};
*/
/*
class Credential {

public:
	std::string userName;
	std::string passwordHash;
	std::string salt;
};
*/

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
	/*
	 * Input query is a valid SQL statement that returns rows
	 * Output is SQL results printed to standard out
	 * Used for testing
	 */
	//Credential getCredential(std::string user);
	/*
	 * Input is username
	 * Output is Credential information stored in Credential class
	 * Used for testing
	 */
	int login(std::string user_name, std::string password,
			std::string &session_id);
	int listUsers(std::string session_id, std::string &user_list); //numbered list with newlines between | for errors set error number and pass error string
	int showWall(std::string session_id, std::string user_name, std::string &wall_contents); //multiples of timestamp\n user posted on users wall\n contents \n\n | | for errors set error number and pass error string
	int postOnWall(std::string session_id, std::string user_name, std::string post_contents, std::string &error_message);
};

/*
global data structure - see structures.h for global structure

Weiyang:
int	create_socket(bool server, ???); //return is positive for success, negative or 0 for failure
int	write(struct packet &pkt); //return is 0 for failed acknowledgement, 1 for acknowledged packet, -1 for failure
int	read(struct packet &pkt); //return is 0 for unsuccessful read or unsuccessful send of ack, 1 for successful read and send of ack, negative for failure
int destroy_socket(???); //return is positive for success, negative or 0 for failure

Michael:
int logout
int getNotification
int updateNotification
int getSocketDescriptor(session_id)
int login(std::string user_name, std::string password, std::string &session_id);
int listUsers(std::string session_id, std::string &user_list);
int showWall(std::string session_id, std::string user_name, std::string &wall_contents);
int postOnWall(std::string session_id, std::string user_name, std::string post_contents, std::string &error_message);
 */

MySQLDatabase::MySQLDatabase() {

	try {
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

/*
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
*/

#endif /* MYSQL_LIB_H_ */
