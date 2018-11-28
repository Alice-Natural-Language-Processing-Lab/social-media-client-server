/*
 * mysql_lib.cpp
 *
 *  Created on: Nov 19, 2018
 *      Author: michael
 */

#include "mysql_lib.h"

MySQLDatabaseDriver::MySQLDatabaseDriver() {

	try {
		driver = get_driver_instance(); //not thread safe
	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

MySQLDatabaseDriver::~MySQLDatabaseDriver() {
}

MySQLDatabaseInterface::MySQLDatabaseInterface(
		MySQLDatabaseDriver* databaseDriver, string server_url,
		string server_username, string server_password,
		string server_database) {

	driver = databaseDriver->driver;
	try {
		con = driver->connect(server_url, server_username, server_password);
		con->setSchema(server_database);
	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

MySQLDatabaseInterface::~MySQLDatabaseInterface() {

	delete con;
}

void MySQLDatabaseInterface::getResults(string query) {

	try {
		stmt = con->createStatement();
		res = stmt->executeQuery(query.c_str());

		printResults(res);

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

int MySQLDatabaseInterface::login(struct packet &pkt, int socket_descriptor) {

	bool valid_session_id = false;
	unsigned int temp_session_id;
	try {
		//check for valid username and password
		pstmt = con->prepareStatement(
				"select * from Users where userName = ? and passwordHash = ?");
		pstmt->setString(1, pkt.contents.username);
		pstmt->setString(2, pkt.contents.password);
		res = pstmt->executeQuery();

		printResults(res);

		if (res->rowsCount() != 1) {
			//username and password does not exist or is incorrect
			pkt.contents.rvcd_cnts =
					"Username and/or password incorrect or does not exist";
			delete pstmt;
			delete res;
			return -1;
		}
		//username and password exists and is correct
		delete pstmt;
		delete res;

		//generate and check for valid session_id
		//this statement doesn't check for existing but invalid session ids. Will eventually run out of session ids
		pstmt = con->prepareStatement(
				"select * from InteractionLog where sessionID = ?");
		while (!valid_session_id) {

			temp_session_id = (unsigned int) round(
					((float) rand() / RAND_MAX) * session_id_max);

			pstmt->setUInt(1, temp_session_id);
			res = pstmt->executeQuery();

			printResults(res);

			if (res->rowsCount() == 0) {
				//session_id doesn't exist in table, use this session id
				valid_session_id = true;
			}
			delete res;
		}
		delete pstmt;

		//insert row in interaction log (can probably make a function for this)
		//how to make sure another thread doesn't generate same sessionid and update concurrently? - mainly relying on low probability, similar approach is used for session ids in the wild
		//insert into InteractionLog (userID, sessionID, logout, socketDescriptor, command)
		//values (1, 23456, 0, 9, "test command");
		pstmt = con->prepareStatement("insert into InteractionLog (userID, sessionID, logout, socketDescriptor, command)");

		//write session id back to packet and return 0

		return 0;

	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		pkt.contents.rvcd_cnts = "SQL Error";
		return -1;
	}

	pkt.contents.rvcd_cnts = "Unknown function error";
	return -1;
}

int MySQLDatabaseInterface::listUsers(struct packet &pkt) {

	/*
	 pkt.sessionId;
	 pkt.contents.username;
	 pkt.contents.rvcd_cnts; //numbered list with newlines between | for errors set error number and pass error string
	 */

	return 0;
}

int MySQLDatabaseInterface::showWall(struct packet &pkt) {

	/*
	 pkt.sessionId;
	 pkt.contents.wallOwner;
	 pkt.contents.rvcd_cnts; //multiples of timestamp\n user posted on users wall\n contents \n\n | | for errors set error number and pass error string
	 */

	return 0;
}

int MySQLDatabaseInterface::postOnWall(struct packet &pkt) {

	return 0;
}

int MySQLDatabaseInterface::hasValidSession(struct packet& pkt) {

	/*
	 * query the database for a matching session from the most recent row
	 * that is sooner than the timeout and isn't logout
	 *
	 * no valid session than rcvd_contents
	 * is just an "invalid session" error message
	 */

	try {
		pstmt =
				con->prepareStatement(
						"SELECT * FROM (SELECT * FROM SocialNetwork.InteractionLog WHERE sessionID = ? ORDER BY TIMESTAMP DESC LIMIT 1) TEMP WHERE ADDTIME(TIMESTAMP, CONCAT('00:', ? ,':00')) > NOW() AND logout <> 1");
		pstmt->setUInt(1, pkt.sessionId);
		pstmt->setInt(2, session_timeout);
		res = pstmt->executeQuery();

		//printResults(res);
		if (res->rowsCount() > 0) {
			//valid session
			delete pstmt;
			delete res;
			return 0;
		} else {
			//invalid session
			pkt.contents.rvcd_cnts = "Invalid Session";
			delete pstmt;
			delete res;
			return -1;
		}

	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -1;
	}

	return -1;
}

void MySQLDatabaseInterface::printResults(sql::ResultSet* result_set) {

	int column_count;

	result_set_meta_data = result_set->getMetaData();
	column_count = result_set_meta_data->getColumnCount();
	std::cout << "\nResults:\n";
	do {
		for (int i = 1; i <= column_count; i++) {
			if (result_set->isBeforeFirst())
				std::cout << "\t" << result_set_meta_data->getColumnName(i);
			else
				std::cout << "\t" << result_set->getString(i);
		}
		std::cout << std::endl;
	} while (result_set->next());
	std::cout << std::endl;
}

Notifications::Notifications(MySQLDatabaseInterface* dbInterface) {
	databaseInterface = dbInterface;
}

Notifications::~Notifications() {

}
