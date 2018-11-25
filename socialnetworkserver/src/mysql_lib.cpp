/*
 * mysql_lib.cpp
 *
 *  Created on: Nov 19, 2018
 *      Author: michael
 */

#include "mysql_lib.h"

Notifications::Notifications(MySQLDatabaseInterface* dbInterface) {
	databaseInterface = dbInterface;
}

Notifications::~Notifications() {

}

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

int MySQLDatabaseInterface::login(struct packet &pkt) {

	/*
	 pkt.contents.username;
	 pkt.contents.password;
	 pkt.sessionId;
	 pkt.contents.rvcd_cnts; //for error return
	 sessionid max is 4,294,967,295
	 */

	return 0;
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

		printResults(res);
		if (res->rowsCount() > 0) {
			//valid session
			return 0;
		} else {
			//invalid session
			pkt.contents.rvcd_cnts = "Invalid Session";
			return -1;
		}

		delete pstmt;
		delete res;
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
