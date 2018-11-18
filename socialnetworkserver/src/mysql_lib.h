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
#include <pthread.h>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "structures.h"

class MySQLDatabaseDriver;
class MySQLDatabaseInterface;
class Notification;

class MySQLDatabaseDriver {
	/*
	 * Call this once is the global space to initialize the MySQLDriver
	 */
private:
	pthread_mutex_t driver_mutex;
public:
	sql::Driver *driver; //deallocates itself, del not necessary

	MySQLDatabaseDriver();
	~MySQLDatabaseDriver();
};

class MySQLDatabaseInterface {
	/*
	 * Call this in each thread that needs to connect to the database.
	 * It handles database connections only within a thread.
	 */
private:
	sql::Driver *driver;
	sql::Connection *con;
	sql::Statement *stmt;
	sql::PreparedStatement *pstmt;
	sql::ResultSet *res;
	sql::ResultSetMetaData *result_set_meta_data;
	unsigned int column_count;

public:
	MySQLDatabaseInterface(MySQLDatabaseDriver databaseDriver,
			string server_url, string server_username, string server_password,
			string server_database);
	~MySQLDatabaseInterface();
	void getResults(string query);
	/*
	 * Input query is a valid SQL statement that returns rows
	 * Output is SQL results printed to standard out
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

MySQLDatabaseDriver::MySQLDatabaseDriver() {

	pthread_mutex_init(&driver_mutex, NULL);
	try {
		pthread_mutex_lock(&driver_mutex);
		driver = get_driver_instance(); //not thread safe
		pthread_mutex_unlock(&driver_mutex);
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
}

MySQLDatabaseDriver::~MySQLDatabaseDriver() {
	pthread_mutex_destroy(&driver_mutex);
}

MySQLDatabaseInterface::MySQLDatabaseInterface(
		MySQLDatabaseDriver databaseDriver, string server_url,
		string server_username, string server_password,
		string server_database) {

	driver = databaseDriver.driver;
	try {
		con = driver->connect(server_url, server_username, server_password);
		con->setSchema(server_database);
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
}

MySQLDatabaseInterface::~MySQLDatabaseInterface() {

	delete con;
}

void MySQLDatabaseInterface::getResults(string query) {

	try {
		stmt = con->createStatement();
		res = stmt->executeQuery(query.c_str());
		result_set_meta_data = res->getMetaData();
		column_count = result_set_meta_data->getColumnCount();
		cout << "\nResults:\n";
		do {
			for (unsigned int i = 1; i <= column_count; i++) {
				if (res->isBeforeFirst())
					cout << "\t" << result_set_meta_data->getColumnName(i);
				else
					cout << "\t" << res->getString(i);
			}
			cout << endl;
		} while (res->next());
		cout << endl;

		delete stmt;
		delete res;
	} catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
}

int MySQLDatabaseInterface::login(struct packet &pkt) {

	pkt.contents.username;
	pkt.contents.password;
	pkt.sessionId;
	pkt.contents.rvcd_cnts; //for error return

	return EXIT_SUCCESS;
}

int MySQLDatabaseInterface::listUsers(struct packet &pkt) {

	pkt.sessionId;
	pkt.contents.username;
	pkt.contents.rvcd_cnts; //numbered list with newlines between | for errors set error number and pass error string

	return EXIT_SUCCESS;
}

int MySQLDatabaseInterface::showWall(struct packet &pkt) {

	pkt.sessionId;
	pkt.contents.wallOwner;
	pkt.contents.rvcd_cnts; //multiples of timestamp\n user posted on users wall\n contents \n\n | | for errors set error number and pass error string

	return EXIT_SUCCESS;
}

int MySQLDatabaseInterface::postOnWall(struct packet &pkt) {

	return EXIT_SUCCESS;
}

#endif /* MYSQL_LIB_H_ */
