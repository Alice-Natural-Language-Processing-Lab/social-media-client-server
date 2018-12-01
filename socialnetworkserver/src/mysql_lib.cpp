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

DatabaseCommandInterface::DatabaseCommandInterface(
		MySQLDatabaseDriver databaseDriver, std::string server_url,
		std::string server_username, std::string server_password,
		std::string server_database) {

	driver = databaseDriver.driver;
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

DatabaseCommandInterface::~DatabaseCommandInterface() {

	delete con;
}

void DatabaseCommandInterface::getResults(std::string query) {

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

int DatabaseCommandInterface::hasValidSession(struct packet& pkt,
		unsigned int* user_id, unsigned int* socket_descriptor) {

	/*
	 * query the database for a matching session from the most recent row
	 * that is sooner than the timeout and isn't logout
	 *
	 * no valid session than rcvd_contents
	 * is just an "invalid session" error message
	 */

	try {
		//This query only looks at sessions. This allows a single user to be logged into multiple sessions.
		pstmt =
				con->prepareStatement(
						"SELECT * FROM (SELECT * FROM SocialNetwork.InteractionLog WHERE "
								"sessionID = ? ORDER BY TIMESTAMP DESC LIMIT 1) TEMP WHERE "
								"ADDTIME(TIMESTAMP, CONCAT('00:', ? ,':00')) > NOW() AND logout <> 1");
		pstmt->setUInt(1, pkt.sessionId);
		pstmt->setInt(2, session_timeout);
		res = pstmt->executeQuery();

		//printResults();
		switch (res->rowsCount()) {
		case 1:
			//valid session
			if (user_id != NULL) {
				res->first();
				*user_id = res->getUInt("userID");
			}
			if (socket_descriptor != NULL) {
				res->first();
				*socket_descriptor = res->getUInt("socketDescriptor");
			}

			delete pstmt;
			delete res;
			return 0;
			break;
		case 0:
			//invalid session
			delete pstmt;
			delete res;
			pkt.contents.rcvd_cnts = "Invalid Session";
			return -1;
			break;
		default:
			delete pstmt;
			delete res;
			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
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

int DatabaseCommandInterface::login(struct packet &pkt,
		unsigned int socket_descriptor) {

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
				//session_id doesn't already exist in table, use this session id
				valid_session_id = true;
			}
			delete res;
		}
		delete pstmt;

		//insert row in interaction log
		//how to make sure another thread doesn't generate same sessionid and update concurrently? - mainly relying on low probability, similar approach is used for session ids in the wild
		if (insertInteractionLog(temp_session_id, false,
				"LOGIN " + pkt.contents.username, temp_user_id,
				socket_descriptor) != 0) {
			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

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

int DatabaseCommandInterface::listUsers(struct packet &pkt) {

	std::string temp;
	try {
		stmt = con->createStatement();
		res = stmt->executeQuery("select userName from Users");

		//printResults();

		if (res->rowsCount() < 1) {
			//SQL not returning users
			delete stmt;
			delete res;

			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		//format results
		while (res->next()) {
			temp += std::to_string(res->getRow()) + " - "
					+ res->getString("userName");
			if (!res->isLast()) {
				temp += "\n";
			}
		}
		delete stmt;
		delete res;

		if (insertInteractionLog(pkt.sessionId, false, "LIST") != 0) {
			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		pkt.contents.rcvd_cnts = temp;

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

int DatabaseCommandInterface::showWall(struct packet &pkt) {

	std::string temp;
	try {
		switch (getUserID(pkt.contents.wallOwner)) {
		case 0:
			//user exists
			break;
		case -1:
			pkt.contents.rcvd_cnts = "User doesn't exist";
			return -1;
			break;
		case -2:
			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
			break;
		}

		//get requested user's wall
		pstmt =
				con->prepareStatement(
						"select timestamp, content, userPostee.userName postee, userPoster.userName "
								"poster from Posts join Users userPostee on userPostee.userID = Posts.posteeUserID "
								"join Users userPoster on userPoster.userID = Posts.posterUserID "
								"where userPostee.userName = ? order by timestamp asc");
		pstmt->setString(1, pkt.contents.wallOwner);
		res = pstmt->executeQuery();

		//printResults();

		if (res->rowsCount() == 0) {
			//no wall contents for user
			temp = "No wall contents";
		} else {
			//generate wall contents
			while (res->next()) {
				temp += res->getString("timestamp") + " - "
						+ res->getString("poster") + " posted on "
						+ res->getString("postee") + "'s wall\n"
						+ res->getString("content");
				if (!res->isLast()) {
					temp += "\n\n";
				}
			}
		}
		delete pstmt;
		delete res;

		if (insertInteractionLog(pkt.sessionId, false,
				"SHOW " + pkt.contents.wallOwner) != 0) {
			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		pkt.contents.rcvd_cnts = temp;

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

int DatabaseCommandInterface::postOnWall(struct packet &pkt) {

	unsigned int poster_id, post_id;
	try {
		/*
		 * determine poster_id and attempt inserting post.
		 * If successful, then postee exists and post has been made
		 */
		if (hasValidSession(pkt, &poster_id) != 0) {

			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		pstmt =
				con->prepareStatement(
						"insert into Posts (posterUserID, posteeUserID, content) "
						"select ?, postee.userID, ? from Users postee where postee.userName = ?");
		pstmt->setUInt(1, poster_id);
		pstmt->setString(2, pkt.contents.post);
		pstmt->setString(3, pkt.contents.postee);

		if (pstmt->executeUpdate() != 1) {
			//postee user doesn't exist
			delete pstmt;
			pkt.contents.rcvd_cnts = "User doesn't exist";
			return -1;
		}

		delete pstmt;
		//post made successfully

		//insert post and users into notifications table
		//get newly created post_id
		stmt = con->createStatement();
		res = stmt->executeQuery("select last_insert_id() as post_id");

		if (res->rowsCount() != 1) {
			delete stmt;
			delete res;

			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		res->first();
		post_id = res->getUInt("post_id");

		delete stmt;
		delete res;

		pstmt =
				con->prepareStatement(
						"insert into Notifications (postID, userID) select ?, userID from Users");
		pstmt->setUInt(1, post_id);

		if (pstmt->executeUpdate() < 1) {
			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		delete pstmt;

		insertInteractionLog(pkt.sessionId, false,
				"POST " + pkt.contents.postee + " " + std::to_string(post_id));

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

int DatabaseCommandInterface::logout(struct packet& pkt) {

	unsigned int user_id;
	std::string user_name;
	try {
		if (hasValidSession(pkt, &user_id) != 0) {

			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		pstmt = con->prepareStatement(
				"select userName from Users where userID = ?");
		pstmt->setUInt(1, user_id);
		res = pstmt->executeQuery();

		if (res->rowsCount() != 1) {
			delete pstmt;
			delete res;

			pkt.contents.rcvd_cnts = "Server Error";
			return -2;
		}

		res->first();
		user_name = res->getString("userName");

		delete pstmt;
		delete res;

		insertInteractionLog(pkt.sessionId, true, "LOGOUT " + user_name);

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

void DatabaseCommandInterface::printResults() {

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

int DatabaseCommandInterface::insertInteractionLog(unsigned int session_id,
		bool logout, std::string command, unsigned int user_id,
		unsigned int socket_descriptor) {

	try {
		if (user_id == 0 || socket_descriptor == 0) {
			//need to query for these if they aren't passed in
			int return_flag;
			packet temp_packet;
			temp_packet.sessionId = session_id;

			if (user_id == 0 && socket_descriptor == 0) {

				return_flag = hasValidSession(temp_packet, &user_id,
						&socket_descriptor);
			} else if (user_id == 0) {
				return_flag = hasValidSession(temp_packet, &user_id);
			} else {
				return_flag = hasValidSession(temp_packet, NULL,
						&socket_descriptor);
			}

			if (return_flag != 0) {
				//session not valid or server error => server error
				return -2;
			}
		}

		pstmt =
				con->prepareStatement(
						"insert into InteractionLog (userID, sessionID, logout, socketDescriptor, command) "
						"values (?, ?, ?, ?, ?)");
		pstmt->setUInt(1, user_id);
		pstmt->setUInt(2, session_id);
		pstmt->setBoolean(3, logout);
		pstmt->setUInt(4, socket_descriptor);
		pstmt->setString(5, command);

		if (pstmt->executeUpdate() != 1) {
			//more or less than 1 row was affected - error condition
			delete pstmt;
			return -2;
		}

		delete pstmt;
		return 0;

	} catch (sql::SQLException &e) {
		std::cout << "# ERR: SQLException in " << __FILE__;
		std::cout << "(" << __FUNCTION__ << ") on line " << __LINE__
				<< std::endl;
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;

		return -2;
	}

	return -2;
}

int DatabaseCommandInterface::getUserID(std::string user_name,
		unsigned int* user_id) {

	try {
		//see if requested user exists
		pstmt = con->prepareStatement("select * from Users where userName = ?");
		pstmt->setString(1, user_name);
		res = pstmt->executeQuery();

		//printResults();

		if (res->rowsCount() == 0) {
			//user doesn't exist
			delete pstmt;
			delete res;

			return -1;
		}
		//user does exist, extract userID
		if (user_id != NULL) {
			res->first();
			*user_id = res->getUInt("userID");
		}

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

		return -2;
	}

	return -2;
}

DatabaseNotificationInterface::DatabaseNotificationInterface(DatabaseCommandInterface* dbInterface) {

	databaseInterface = dbInterface;
}

DatabaseNotificationInterface::~DatabaseNotificationInterface() {

}

int DatabaseNotificationInterface::getNotifications(void) {
	/*
	 *This query should give me users that are eligible for notifications
	 *
	 select OnlineUsers.socketDescriptor, Notifications.notificationID,
Posts.content, Poster.userName poster, Postee.userName postee
from (select IntLog2.userID, IntLog1.socketDescriptor from
(select userID, max(timestamp) maxTimestamp from InteractionLog group by userID) IntLog2 join
(select userID, timestamp, logout, socketDescriptor from InteractionLog) IntLog1
on IntLog1.userID = IntLog2.userID
and IntLog1.timestamp = IntLog2.maxTimestamp
where logout <>1
and ADDTIME(TIMESTAMP, CONCAT('00:', 50 ,':00')) > NOW()) OnlineUsers
join Notifications on OnlineUsers.userID = Notifications.userID
join Posts on Posts.postID = Notifications.postID
join Users Poster on Poster.userID = Posts.posterUserID
join Users Postee on Postee.userID = Posts.posteeUserID
where Notifications.readFlag = 0
	 */


	return 0;
}

int DatabaseNotificationInterface::next(void) {

	return 0;
}

int DatabaseNotificationInterface::sendNotification(struct packet& pkt) {

	return 0;
}

int DatabaseNotificationInterface::markRead(void) {

	return 0;
}
