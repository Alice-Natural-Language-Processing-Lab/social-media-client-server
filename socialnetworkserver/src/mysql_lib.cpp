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
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
}

MySQLDatabaseDriver::~MySQLDatabaseDriver() {
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
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
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
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
	}
}

int MySQLDatabaseInterface::login(struct packet &pkt) {

	/*
	 pkt.contents.username;
	 pkt.contents.password;
	 pkt.sessionId;
	 pkt.contents.rvcd_cnts; //for error return
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
