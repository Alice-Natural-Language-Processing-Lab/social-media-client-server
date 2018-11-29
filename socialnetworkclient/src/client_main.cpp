
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <tr1/functional>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <unordered_map>
#include <iostream>
#include <pthread.h>
#include "structures.h"
#include "func_lib.h"
#include "networking.h"

using namespace std;
#define DEBUG
#define ERR_LEN			256

string username;
unsigned int sessionID;
time_t now;
char *error = (char *)malloc(sizeof(char) * ERR_LEN);

const char * getCommand(int enumVal)
{
  return commandList[enumVal];
}

void getLoginInfo(string &pw);
int enterLoginMode(string servername, int serverport);
int enterWebBrowserMode(string servername, int serverport);
int getAddrInfo(string host, string port, struct addrinfo **serv_info);

int main(int argc, char *argv[])
{
	string servername = "localhost";
	int serverport = 5354;
	int ret = 0;

	switch (argc)
	{
	case 1:
			break;
	case 2:
			servername = argv[1];
			break;
	case 3:
			servername = argv[1];
			serverport = stoi(argv[2]);
			break;
	default:
			printf("Error: Usage is ./[executable] [hostname] [port]\n");
			return -1;
	}
	ret = enterLoginMode(servername, serverport);
	if (ret < 0)
	{
		printf("Error (processClient): Client processing failed\n");
		now = time(NULL);
		return -1;
	}
	return 0;
}

/**
 * enterLoginMode() - accept user request to exit or login
 * servername: ip of endpoint
 * serverport: port number of server
 * return 0 or -1
 **/
int enterLoginMode(string servername, int serverport)
{
    int option = 0;
    string input;
    while (1)
    {
   	    cout<<"Enter an Option (1 or 2)"<<endl;
        cout<<"1. Login\n2. Exit"<<endl;
        getline(std::cin, input);
        if (!input.length())
        	break;
        option = atoi(input.c_str());
        if (option == 1)
        {
            enterWebBrowserMode(servername, serverport);
            break;
        }
        else if (option == 2)
        {
            cout<<"Gracefully exiting"<<endl;
            break;
        }
        else
        {
            cout<<"Invalid option. Try again!!"<<endl;
        }
    }
    return 0;
}


int enterWebBrowserMode(string servername, int serverport)
{
    int  sock_fd;
    struct packet *resp = (struct packet *)malloc(sizeof(struct packet));
    char *buffer = (char *)resp;
    int resp_len;
    string pw;
    int ret;
    sock_fd = create_client_socket(servername, serverport);
    if (sock_fd < 0)
    {
    	printf("Error (create_client_socket): Client Socket creation failed\n");
    	return -1;
    }
    DEBUG("Socket connected \n");

    /*Getting username and password from stdin*/
    getLoginInfo(pw);

    /*Creating and sending packet*/
    sendPacket(sock_fd, LOGIN, username, pw);

    /* create 2 threads to handle read and write */
    pthread_t th1, th2;
    pthread_attr_t ta;
    pthread_attr_init(&ta);
    pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);
    ret = pthread_create(&th1, &ta, (void * (*) (void *)) readThread, (void *)((long) sock_fd));
    if (ret < 0)
    {
        perror("\nRead Thread create error");
        return -1;
    }
    DEBUG("Read thread created ret = %d\n", ret);
    ret = pthread_create(&th2, &ta, (void * (*) (void *)) writeThread, (void *)((long) sock_fd));
    if (ret < 0)
    {
        perror("\nWrite Thread create error");
        return -1;
    }
    DEBUG("Write thread created ret = %d\n", ret);
    while (1) {}
    return 0;
}

void getLoginInfo(string &pw)
{
	std::tr1::hash<string> hashfun;
	string password;

    cout<<"Enter User Name:";
    cin>>username;
    password = getpass("\nEnter Password:");
    cout<<endl;
    pw = to_string(hashfun(password));
    DEBUG("Hashed pw is %s\n", pw.c_str());
    return;
}

/**
 * getAddrInfo() - get addr info of server
 * host: server host information
 * port: server port
 * serv_info: to store server information
 * return addinfo status
 */
int getAddrInfo(string host, string port, struct addrinfo **serv_info)
{
	struct addrinfo hints;
	int addr_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family		= AF_INET;
	hints.ai_socktype	= SOCK_STREAM;
	hints.ai_protocol	= 0;
	hints.ai_flags		= 0;
	addr_info		= getaddrinfo(host.c_str(), port.c_str(), &hints, serv_info);
	return addr_info;
}
