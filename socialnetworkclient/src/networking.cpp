/*
 * networking.cpp
 *
 *  Created on: Nov 8, 2018
 *      Author: pan
 */
#include "networking.h"

#define LISTEN_QUEUE_LENGTH 15
#define DEBUG printf

int packetSeqNum = 0;

int create_server_socket(int portNum) {
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if(socketfd < 0) {
		fprintf(stderr, "Failed to create Listening Socket; Error Message: %s\n", strerror(errno));
		return -1;
	}

	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr)); //Clear structure
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(portNum);

	if(bind(socketfd,(struct sockaddr *)&serverAddr , sizeof(serverAddr)) < 0) {
		fprintf(stderr, "Failed to Bind; Error Message: %s\n", strerror(errno));
		return -2;
	}

	/* Listen on the socket */
	if(listen(socketfd, LISTEN_QUEUE_LENGTH) < 0) {
		fprintf(stderr, "Failed to Listen; Error Message: %s\n", strerror(errno));
		return -3;
	}

	return socketfd;
}

int create_client_socket(string serverName, int portNum) {
	DEBUG("Inside create client socket\n");
	struct addrinfo *serverInfo;
	string port = to_string(portNum);

	int error;
	error = getaddrinfo(serverName.c_str(), port.c_str(), NULL, &serverInfo);
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
			continue;
		}
		//connect socket
		struct sockaddr_in serverAddress;
		memset(&serverAddress, 0, sizeof(serverAddress));
		memcpy(&serverAddress, currentInfo->ai_addr, currentInfo->ai_addrlen);
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_port = htons(portNum);

		if(connect(socketfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) > -1) {
			break; //success
		}
	}

	return socketfd;
}

int accept_socket(int socketfd) {
	struct sockaddr_in clientAddr;
	unsigned int clientSize = sizeof(clientAddr);
	int slaveSocket = accept(socketfd, (struct sockaddr *)&clientAddr, &clientSize);
	if(slaveSocket < 0) {
		fprintf(stderr, "Failed to Accept Socket; Error Message: %s\n", strerror(errno));
		return errno;
	}
	return slaveSocket;
}

int destroy_socket(int socketfd) {
	if(close(socketfd) < 0) {
		fprintf(stderr, "Failed to Close Socket; Error Message: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}


/*
int write_socket(int socketfd, struct packet &pkt) {
	int contentLength = pkt.contents.username.length() + pkt.contents.password.length() + pkt.contents.postee.length() + pkt.contents.post.length() + pkt.contents.wallOwner.length() + pkt.contents.rvcd_cnts.length();
	pkt.content_len = (unsigned int) contentLength;
	pkt.req_num = packetSeqNum;
	packetSeqNum++;
	string pktString = "";
	sprintf(pktString, "content_len:%s,", std::to_string(pkt.content_len));
	pktString.append()

}

int read_socket(int socketfd, struct packet &pkt) {
	int received = 0;
    int bytes = 0;
    do {
    	bytes = read(socketfd, request+received, total-received);
    	if(bytes < 0) {
    		printf("Error when reading.");
    		return -5;
    	}
    	if(bytes == 0) {
    		break;
    	}
    	received += bytes;
    } while(received < total);

}
*/
