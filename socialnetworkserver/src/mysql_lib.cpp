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

void MySQLDatabaseInterface::printResults() {

	int column_count, initial_row = res->getRow();
	res->beforeFirst();
	result_set_meta_data = res->getMetaData();
	column_count = result_set_meta_data->getColumnCount();
	std::cout << "\nResults:\n";
	do {
		for (int i = 1; i <= column_count; i++) {
			if (res->isBeforeFirst())
				std::cout << "\t" << result_set_meta_data->getColumnName(i);
			else
				std::cout << "\t" << res->getString(i);
		}
		std::cout << std::endl;
	} while (res->next());
	std::cout << std::endl;
	res->absolute(initial_row);
}

MySQLDatabaseInterface::MySQLDatabaseInterface(
		MySQLDatabaseDriver* databaseDriver, std::string server_url,
		std::string server_username, std::string server_password,
		std::string server_database) {

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

void MySQLDatabaseInterface::getResults(std::string query) {

	try {
		stmt = con->createStatement();
		res = stmt->executeQuery(query.c_str());

		printResults();

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

		//printResults();
		if (res->rowsCount() > 0) {
			//valid session
			delete pstmt;
			delete res;
			return 0;
		} else {
			//invalid session
			pkt.contents.rcvd_cnts = "Invalid Session";
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

		pkt.contents.rcvd_cnts = "Server Error";
		return -2;
	}

	pkt.contents.rcvd_cnts = "Server Error";
	return -2;
}

int MySQLDatabaseInterface::login(struct packet &pkt, int socket_descriptor) {

	bool valid_session_id = false;
	unsigned int temp_session_id, temp_user_id;
	srand(time(NULL));

	try {
		//check for valid username and password
		pstmt = con->prepareStatement(
				"select * from Users where userName = ? and passwordHash = ?");
		pstmt->setString(1, pkt.contents.username);
		pstmt->setString(2, pkt.contents.password);
		res = pstmt->executeQuery();

		//printResults();

		if (res->rowsCount() != 1) {
			//username and password does not exist or is incorrect
			pkt.contents.rcvd_cnts =
					"Username and/or password incorrect or does not exist";
			delete pstmt;
			delete res;
			return -1;
		}
		//username and password exists and is correct
		res->first();
		temp_user_id = res->getUInt("userID");
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

			//printResults();

			if (res->rowsCount() == 0) {
				//session_id doesn't exist in table, use this session id
				valid_session_id = true;
			}
			delete res;
		}
		delete pstmt;

		//insert row in interaction log (can probably make a function for this)
		//how to make sure another thread doesn't generate same sessionid and update concurrently? - mainly relying on low probability, similar approach is used for session ids in the wild
		pstmt =
				con->prepareStatement(
						"insert into InteractionLog (userID, sessionID, logout, socketDescriptor, command) values (?, ?, 0, ?, ?)");
		pstmt->setUInt(1, temp_user_id);
		pstmt->setUInt(2, temp_session_id);
		pstmt->setInt(3, socket_descriptor);
		pstmt->setString(4, "LOGIN " + pkt.contents.username);
		pstmt->executeUpdate();
		delete pstmt;

		//write session id back to packet and return 0
		pkt.sessionId = temp_session_id;

		return 0;

	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		pkt.contents.rcvd_cnts = "Server Error";
		return -2;
	}

	pkt.contents.rcvd_cnts = "Server Error";
	return -2;
}

int MySQLDatabaseInterface::listUsers(struct packet &pkt) {

	std::string temp;
	try {
		stmt = con->createStatement();
		res = stmt->executeQuery("select userName from Users");

		//printResults();

		while (res->next()) {
			temp += std::to_string(res->getRow()) + " - "
					+ res->getString("userName");
			if (!res->isLast()) {
				temp += "\n";
			}
		}

		pkt.contents.rcvd_cnts = temp;

		delete stmt;
		delete res;

		return 0;

	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		pkt.contents.rcvd_cnts = "Server Error";
		return -2;
	}

	pkt.contents.rcvd_cnts = "Server Error";
	return -2;
}

int MySQLDatabaseInterface::showWall(struct packet &pkt) {

	std::string temp;
	try {
		//see if requested user exists
		pstmt = con->prepareStatement("select * from Users where userName = ?");
		pstmt->setString(1, pkt.contents.wallOwner);
		res = pstmt->executeQuery();

		//printResults();

		if (res->rowsCount() == 0) {
			//user doesn't exist
			pkt.contents.rcvd_cnts = "User doesn't exist";

			delete pstmt;
			delete res;
			return -1;
		}

		delete pstmt;
		delete res;

		//get requested user's wall
		pstmt =
				con->prepareStatement(
						"select timestamp, content, userPostee.userName postee, userPoster.userName poster from Posts join Users userPostee on userPostee.userID = Posts.posteeUserID join Users userPoster on userPoster.userID = Posts.posterUserID where userPostee.userName = ? order by timestamp asc");
		pstmt->setString(1, pkt.contents.wallOwner);
		res = pstmt->executeQuery();

		//printResults();

		if (res->rowsCount() == 0) {
			//no wall contents for user
			pkt.contents.rcvd_cnts = "No wall contents";

			delete pstmt;
			delete res;
			return 0;
		}

		while (res->next()) {
			temp += res->getString("timestamp") + " - "
					+ res->getString("poster") + " posted on "
					+ res->getString("postee") + "'s wall\n"
					+ res->getString("content");
			if (!res->isLast()) {
				temp += "\n\n";
			}
		}

		pkt.contents.rcvd_cnts = temp;

		delete pstmt;
		delete res;

		return 0;

	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		pkt.contents.rcvd_cnts = "Server Error";
		return -2;
	}

	pkt.contents.rcvd_cnts = "Server Error";
	return -2;
}

int MySQLDatabaseInterface::postOnWall(struct packet &pkt) {

	return 0;
}

int MySQLDatabaseInterface::logout(struct packet& pkt) {

	return 0;
}

Notifications::Notifications(MySQLDatabaseInterface* dbInterface) {
	databaseInterface = dbInterface;
}

Notifications::~Notifications() {

}
