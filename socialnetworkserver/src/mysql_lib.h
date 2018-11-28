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
#include <climits>
#include <cstdlib>
#include <math.h>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "structures.h"

class MySQLDatabaseDriver {
	/*
	 * Call this once in the global space to initialize the MySQLDriver
	 */
public:
	sql::Driver *driver; //deallocates itself, del not necessary

	MySQLDatabaseDriver();
	~MySQLDatabaseDriver();
};

class MySQLDatabaseInterface {
	/*
	 * Call this in each client handler thread that needs to connect to the database.
	 * It handles database connections only within a thread.
	 */
private:
	sql::Driver* driver;
	sql::Connection* con;
	sql::Statement* stmt;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* res;
	sql::ResultSetMetaData* result_set_meta_data;

	void printResults();

public:
	int session_timeout = 15; // in minutes between 0 and 59
	unsigned int session_id_max = UINT_MAX;

	MySQLDatabaseInterface(MySQLDatabaseDriver* databaseDriver,
			std::string server_url, std::string server_username,
			std::string server_password, std::string server_database);
	~MySQLDatabaseInterface();

	void getResults(std::string query);
	/*
	 * Input query is a valid SQL statement that returns rows
	 * Output is SQL results printed to standard out
	 * Used for testing
	 */

	/*
	 * the following functions take the request packet input, perform SQL queries and then overwrite the request
	 * packet with the corresponding response packet. They return 0 if successful and
	 * -1 if unsuccessful. If unsuccessful, rcvd_cnts will also contain an error message.
	 */

	int hasValidSession(struct packet& pkt);
	/*
	 * This function checks if the session in the packet is valid based on session_timeout,
	 * returns:
	 * 0 if valid
	 * -1 for invalid session and modifies packet to have error message
	 * -2 for server error and modifies packet to have error message
	 */

	int login(struct packet& pkt, int socket_descriptor);
	/*
	 * Checks if username and password exist in the table. If so, generates
	 * a valid sessionID and writes that ID to the packet and returns 0.
	 * If not, writes an error message to received contents and returns -1.
	 * If server error, writes an error message to received contents and returns -2.
	 */

	int listUsers(struct packet& pkt);
	/*
	 * Queries database for list of all users. Writes numbered list of users
	 * with newlines between users to rcvd_cnts.
	 * Ex:
	 * 1. alice
	 * 2. bob
	 *
	 * Returns 0 if successful, or -2 if server error and writes error message to rcvd_cnts
	 */

	int showWall(struct packet& pkt);
	/*
	 * Queries database for list of posts on a user's wall. Writes a formatted
	 * string of posts to rcvd_cnts.
	 * Ex:
	 * timestamp - Alice posted on Bob's wall
	 * Oh my god! Politics!
	 *
	 * timestamp - Claire posted on Bob's wall
	 * I know, right!
	 *
	 * Returns 0 if successful,
	 * or -1 if unsuccessful and writes error message to rcvd_cnts,
	 * or -2 if server error and writes error message to rcvd_cnts
	 */

	int postOnWall(struct packet& pkt);
	int logout(struct packet& pkt);
};

class Notifications {
	/*
	 * The Notification object handles querying the database for notifications,
	 * iterating through the notifications, generating a packet to send,
	 * and updating the database if a notification is successfully sent to the client.
	 *
	 * It requires a MySQLDatabaseInteface to have been initialized and passed to it.
	 *
	 * This object should only be used within the notifications thread
	 */
private:
	MySQLDatabaseInterface* databaseInterface;

public:
	Notifications(MySQLDatabaseInterface* dbInterface);
	~Notifications();

	int getNotifications(void);
	/*
	 * queries the database for notifications to process. Returns the number of notifications
	 * to process.
	 */

	bool next(void);
	/*
	 * Iterates the Notification object to the next entry. Returns true if the entry exists.
	 * Starts on an empty "0th" entry so much be called once to get to the first entry.
	 */

	int sendNotification(struct packet& pkt);
	/*
	 * Generates the notification packet and returns the socket descriptor to send it to.
	 * Returns -1 if fails.
	 */
	int markRead(void);
	/*
	 * Marks the current notification as read (acknowledged by client code).
	 * Returns 0 if successful.
	 */
};

#endif /* MYSQL_LIB_H_ */
