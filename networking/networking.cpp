/*
 * networking.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: pan
 */
#include "networking.h"

unsigned int packetSeqNum = 0;
bool isServer = false;

int create_server_socket(int portNum) {
	isServer = true;
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if(socketfd < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "Failed to create Listening Socket; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -1;
	}

	//set reuse addr and port
	int enable = 1;
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "setsockopt(SO_REUSEADDR) failed; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -4;
	}
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "setsockopt(SO_REUSEPORT) failed; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -4;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr)); //Clear structure
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(portNum);

	if(bind(socketfd,(struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "Failed to Bind; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -2;
	}

	/* Listen on the socket */
	if(listen(socketfd, LISTEN_QUEUE_LENGTH) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "Failed to Listen; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -3;
	}
/*
	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	if(setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "setsockopt(TIMEOUT) failed; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return errno;
	}
*/
	return socketfd;
}

int create_client_socket(string serverName, int portNum) {
	struct addrinfo *serverInfo;
	struct addrinfo hints;
	string portString = to_string(portNum);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = 0;

	int error;
	error = getaddrinfo(serverName.c_str(), portString.c_str(), NULL, &serverInfo);
	if(error != 0) {
		fprintf(stderr, "Failed to find addrinfo for the URL: %s, error: %s\n", serverName.c_str(), gai_strerror(error));
		return -1;
	}

	struct addrinfo *currentInfo;
	int socketfd = 0;
	for(currentInfo = serverInfo; currentInfo != NULL; currentInfo = currentInfo->ai_next) {
		//create socket
		socketfd = socket(currentInfo->ai_family, currentInfo->ai_socktype, currentInfo->ai_protocol);
		if(socketfd < 0) {
			char errorMessage[ERR_LEN];
			fprintf(stderr, "Failed to Create Socket; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
			return -2;
		}
		//connect socket
		if(connect(socketfd, currentInfo->ai_addr, currentInfo->ai_addrlen) > -1) {
			break; //success
		}
		close(socketfd);
		socketfd = -3;		//none success
	}

	//set reuse addr and port
	int enable = 1;
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
		char errorMessage[ERR_LEN];
		strerror_r(errno, errorMessage, ERR_LEN);
		fprintf(stderr, "setsockopt(SO_REUSEADDR) failed; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -4;
	}
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0) {
		char errorMessage[ERR_LEN];
		strerror_r(errno, errorMessage, ERR_LEN);
		fprintf(stderr, "setsockopt(SO_REUSEPORT) failed; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -4;
	}
	
	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	if(setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "setsockopt(TIMEOUT) failed; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -5;
	}

	return socketfd;
}

int accept_socket(int socketfd) {
	struct sockaddr_in clientAddr;
	unsigned int clientSize = sizeof(clientAddr);
	int slaveSocket = accept(socketfd, (struct sockaddr *)&clientAddr, &clientSize);
	if(slaveSocket < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "Failed to Accept Socket; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -1;
	}

	struct timeval tv;
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	if(setsockopt(slaveSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "setsockopt(TIMEOUT) failed; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -2;
	}

	return slaveSocket;
}

int destroy_socket(int socketfd) {
	if(close(socketfd) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "Failed to Close Socket; Error Message: %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -1;
	}
	return 0;
}

int write_socket_helper(int socketfd, struct packet &pkt) {
	printf("socketfd: %d\n", socketfd);

	char pktString[MAX_PACKET_LEN];

	strcpy(pktString, "content_len:");	//12, the length of fixed format of packet
	strcat(pktString, to_string(pkt.content_len).c_str());
	strcat(pktString, ",cmd_code:");	//10
	strcat(pktString, to_string(pkt.cmd_code).c_str());
	strcat(pktString, ",req_num:");		//9
	strcat(pktString, to_string(pkt.req_num).c_str());
	strcat(pktString, ",sessionId:");	//11
	strcat(pktString, to_string(pkt.sessionId).c_str());
	strcat(pktString, ",username:");	//10
	strcat(pktString, pkt.contents.username.c_str());
	strcat(pktString, ",password:");	//10
	strcat(pktString, pkt.contents.password.c_str());
	strcat(pktString, ",postee:");		//8
	strcat(pktString, pkt.contents.postee.c_str());
	strcat(pktString, ",post:");		//6
	strcat(pktString, pkt.contents.post.c_str());
	strcat(pktString, ",wallOwner:");	//11
	strcat(pktString, pkt.contents.wallOwner.c_str());
	strcat(pktString, ",rcvd_cnts:");	//11
	strcat(pktString, pkt.contents.rcvd_cnts.c_str());
	//total is 12+10+9+11+10+10+8+6+11+11 = 98

	//For Debug Purpose
	printf("stirng: %s\nstringLength: %d\n", pktString, (int)strlen(pktString));

	if(write(socketfd, pktString, strlen(pktString)) < 0) {
		char errorMessage[ERR_LEN];
		fprintf(stderr, "Error (read): %s\n", strerror_r(errno, errorMessage, ERR_LEN));
		return -1;
	}
	return 0;
}

int read_socket_helper(int socketfd, struct packet &pkt) {
	printf("socketfd: %d\n", socketfd);

	int byteRead = 0;
	int totalRead = 0;
	int startIndex = -1;
	int endIndex = -1;
	char errorMessage[ERR_LEN];
	char *buffer = (char *)malloc(sizeof(char) * MAX_PACKET_LEN);
	char *bufferHead = buffer;
	int packetLength = MAX_PACKET_LEN;

	// Read each request stream repeatedly
	while (1 == 1)
	{
		byteRead = read(socketfd, buffer, packetLength);
		if (byteRead < 0)
		{
			fprintf(stderr, "Error (read): %s\n", strerror_r(errno, errorMessage, ERR_LEN));
			free(bufferHead);
			return -2;
		}
		buffer += byteRead;
		totalRead += byteRead;

		//Break the loop when the request structure is read completely
		string temp(bufferHead);
		startIndex = temp.find("content_len:");
		endIndex = temp.find(",cmd_code");
		if(startIndex != -1 && endIndex != -1) {
			string contentLengthString = temp.substr(startIndex + 12, endIndex - startIndex - 12);
			packetLength = 98 + contentLengthString.length() + stoi(contentLengthString);
		}
		if (byteRead == 0 || packetLength <= totalRead)
			break;
	}
	if(bufferHead == NULL || totalRead == 0) {
		//fprintf(stderr, "Packet Read is NULL\n");
		free(bufferHead);
		return 0;
	}

	string pktString(bufferHead);
	free(bufferHead);

	startIndex = pktString.find("content_len:");	//first ':'
	endIndex = pktString.find(",cmd_code:");		//first ','
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	string component = pktString.substr(startIndex + 12, endIndex - startIndex - 12);
	pkt.content_len = (unsigned int) stoi(component);
	//prints are for debug purposes
	printf("content_len:%u\n", pkt.content_len);

	startIndex = pktString.find(",cmd_code:", endIndex);	//next component start
	endIndex = pktString.find(",req_num:", startIndex);	//next component end
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 10, endIndex - startIndex - 10);
	pkt.cmd_code = static_cast<commands>(stoi(component));
	printf("cmd_code:%d\n", pkt.cmd_code);

	startIndex = pktString.find(",req_num:", endIndex);
	endIndex = pktString.find(",sessionId:", startIndex);
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 9, endIndex - startIndex - 9);
	pkt.req_num = (unsigned int) stoul(component);
	printf("req_num:%u\n", pkt.req_num);

	startIndex = pktString.find(",sessionId:", endIndex);
	endIndex = pktString.find(",username:", startIndex);
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 11, endIndex - startIndex - 11);
	pkt.sessionId = (unsigned int) stoul(component);
	printf("sessionId:%u\n", pkt.sessionId);

	startIndex = pktString.find(",username:", endIndex);
	endIndex = pktString.find(",password:", startIndex);
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 10, endIndex - startIndex - 10);
	if (component.length() > 0)
		pkt.contents.username = component;
	printf("username:%s\n", pkt.contents.username.c_str());

	startIndex = pktString.find(",password:", endIndex);
	endIndex = pktString.find(",postee:", startIndex);
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 10, endIndex - startIndex - 10);
	if (component.length() > 0)
		pkt.contents.password = component;
	printf("password:%s\n", pkt.contents.password.c_str());

	startIndex = pktString.find(",postee:", endIndex);
	endIndex = pktString.find(",post:", startIndex);
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 8, endIndex - startIndex - 8);
	if (component.length() > 0)
		pkt.contents.postee = component;
	printf("postee:%s\n", pkt.contents.postee.c_str());

	startIndex = pktString.find(",post:", endIndex);
	endIndex = pktString.find(",wallOwner:", startIndex);
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 6, endIndex - startIndex - 6);
	if (component.length() > 0)
		pkt.contents.post = component;
	printf("post:%s\n", pkt.contents.post.c_str());

	startIndex = pktString.find(",wallOwner:", endIndex);
	endIndex = pktString.find(",rcvd_cnts:", startIndex);
	if(startIndex == -1 || endIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 11, endIndex - startIndex - 11);
	if (component.length() > 0)
		pkt.contents.wallOwner = component;
	printf("wallOwner:%s\n", pkt.contents.wallOwner.c_str());

	startIndex = pktString.find(",rcvd_cnts:", endIndex);
	if(startIndex == -1) {
		fprintf(stderr, "Packer Format Wrong\n");
		return -1;
	}
	component = pktString.substr(startIndex + 11, packetLength - startIndex - 11);
	if (component.length() > 0)
		pkt.contents.rcvd_cnts = component;
	printf("rcvd_cnts:%s\n", pkt.contents.rcvd_cnts.c_str());
	
	return totalRead;
}

int write_socket(int socketfd, struct packet &pkt) {
	if((isServer && pkt.cmd_code == NOTIFY) || (!isServer && pkt.cmd_code != ACK && pkt.cmd_code != NOTIFY)) {	//if the packet is a new request, assign a req-num to it
		pkt.req_num = packetSeqNum;
		packetSeqNum++;
	}

	int contentLength = to_string(pkt.cmd_code).length() + to_string(pkt.req_num).length() + to_string(pkt.sessionId).length() + pkt.contents.username.length() + pkt.contents.password.length() + pkt.contents.postee.length() + pkt.contents.post.length() + pkt.contents.wallOwner.length() + pkt.contents.rcvd_cnts.length();
	pkt.content_len = (unsigned int) contentLength;

	int writeError = write_socket_helper(socketfd, pkt);
	if(writeError < 0)
		return writeError;

	if(pkt.cmd_code == ACK)
		return 0;
	struct packet ackPkt;
	int readError = read_socket_helper(socketfd, ackPkt);
	if(readError <= 0) {
		fprintf(stderr, "Failed to Read ACK Packet\n");
		return -2;
	}
	if(ackPkt.content_len != pkt.content_len || ackPkt.cmd_code != ACK || ackPkt.req_num != pkt.req_num || ackPkt.sessionId != pkt.sessionId) {
		fprintf(stderr, "ACK Packet does not match\n");
		return -3;
	}


	return 0;
}

int read_socket(int socketfd, struct packet &pkt) {
	int readError = read_socket_helper(socketfd, pkt);
	if(readError <= 0)	//error in reading
		return readError;

	if(pkt.cmd_code == ACK) {
		fprintf(stderr, "Recieved a ACK Packet, this should not be happening\n");
		return -4;
	}

	struct packet ackPkt;
	ackPkt.content_len = pkt.content_len;
	ackPkt.cmd_code = ACK;
	ackPkt.req_num = pkt.req_num;
    ackPkt.sessionId = pkt.sessionId;
    ackPkt.contents.username = pkt.contents.username;
	ackPkt.contents.password = pkt.contents.password;
	ackPkt.contents.postee = pkt.contents.postee;
	ackPkt.contents.post = pkt.contents.post;
	ackPkt.contents.wallOwner = pkt.contents.wallOwner;
	ackPkt.contents.rcvd_cnts = pkt.contents.rcvd_cnts;

	int writeError = write_socket_helper(socketfd, ackPkt);
	if(writeError == 0)
		return readError;
	else {
		fprintf(stderr, "Failed to Send ACK Packet\n");
		return -3;
	}


	return 0;
}
