/*
 * structures.h
 *
 *  Created on: Oct 24, 2018
 *      Author: pournami
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_

using namespace std;

#define USR_NAME_LEN	25
#define PW_LEN	20
#define POSTEE_LEN 20
#define OWNER_LEN	20
#define POST_LEN 100
#define MAX_CONT_LEN 100

enum commands {
    LOGIN,
    LOGOUT,
    POST,
    SHOW,
    LIST
};

struct content {
	char username[USR_NAME_LEN];
	char password[PW_LEN];
	char postee[POSTEE_LEN];
	char post[POST_LEN];
	char wallOwner[OWNER_LEN];
	char rvcd_cnts[MAX_CONT_LEN];
};
/*
struct content {
	string username;
	string password;
	string postee;
	string post;
	string wallOwner;
	string rvcd_cnts;
};
*/
/**
 * request - structure to store the request message
 * command: to store the command portion of the request
 */

struct packet {
    unsigned int content_len;
    enum commands cmd_code;
    unsigned int req_num;
    unsigned int sessionId;
    struct content contents;
};



#endif /* STRUCTURES_H_ */
