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

#include "structures.h"

/*
 enum function_returns {
 SUCCESS = 0, INVALID_SESSION_ID = -1, INVALID_CREDENTIALS = -2
 };

 class Credential {

 public:
 std::string userName;
 std::string passwordHash;
 std::string salt;
 };
 */

class MySQLDatabase;
class Notification;

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

	/*
	 * the following functions take the request packet input, perform SQL queries and then overwrite the request
	 * packet with the corresponding response packet. They return EXIT_SUCCESS if successful and
	 * EXIT_FAILURE if unsuccessful. If unsuccessful, rvcd_cnts will also contain an error message.
	 */
	int login(struct packet &pkt);
	int listUsers(struct packet &pkt);
	int showWall(struct packet &pkt);
	int postOnWall(struct packet &pkt);
	int logout(struct packet &pkt);

	/*
	 * The following functions are called in the notifications thread.
	 */
	//Notification object not created yet. Still to be done. But function names won't change.
	Notification getNotifications(void); //returns a Notification object to iterate through
	bool next(void); //iterates the Notification object to the next entry. Returns true if the entry exists.
	int sendNotification(struct packet &pkt); //generates the notification packet and returns the socket descriptor to send it to. Returns -1 if fails
	int markRead(void); //marks the current notification as read (acknowledged by client code). Returns EXIT_SUCCESS if successful

};

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

int MySQLDatabase::login(struct packet &pkt) {

	pkt.contents.username;
	pkt.contents.password;
	pkt.sessionId;
	pkt.contents.rvcd_cnts; //for error return

	return EXIT_SUCCESS;
}

int MySQLDatabase::listUsers(struct packet &pkt) {

	pkt.sessionId;
	pkt.contents.username;
	pkt.contents.rvcd_cnts; //numbered list with newlines between | for errors set error number and pass error string

	return EXIT_SUCCESS;
}

int MySQLDatabase::showWall(struct packet &pkt) {

	pkt.sessionId;
	pkt.contents.wallOwner;
	pkt.contents.rvcd_cnts; //multiples of timestamp\n user posted on users wall\n contents \n\n | | for errors set error number and pass error string

	return EXIT_SUCCESS;
}

int MySQLDatabase::postOnWall(struct packet &pkt) {

	return EXIT_SUCCESS;
}

#endif /* MYSQL_LIB_H_ */
