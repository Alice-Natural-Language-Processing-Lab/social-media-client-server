/*
 * client.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: jagdeep
 */



#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#include <unordered_map>
#include <iostream>
#include <pthread.h>
using namespace std;

#define DEBUG	printf
int req_num;


enum commands {
    LOGIN,
    LOGOUT,
    POST,
    SHOW,
    LIST
};

struct packet {
    unsigned int content_len;
    enum commands cmd_code;
    unsigned int req_num;
    unordered_map <string, string> contents;
};

int getAddrInfo(string host, string port, struct addrinfo **serv_info);
int enterLoginMode(string servername, string serverport);
int enterWebBrowserMode(string servername, string serverport);
void getLoginInfo(string &username, string &pw);
void readThread(int sock_fd);
void listUsers(int sock_fd);
void post();
void printCmdList();
void writeThread(int sock_fd);
int sendPacket(int sock_fd, enum commands cmd_code, string key, string value);

int main(int argc, char *argv[])
{
	string servername = "localhost";
	string serverport = "5354";
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
			serverport = argv[2];
			break;
	default:
			printf("Error: Usage is ./[executable] [hostname] [port]\n");
			return -1;
	}
	ret = enterLoginMode(servername, serverport);
	if (ret < 0)
	{
		printf("Error (processClient): Client processing failed\n");
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
int enterLoginMode(string servername, string serverport)
{
    int option = 0;
    while (1)
    {
   	    cout<<"Enter an Option (1 or 2)"<<endl;
        cout<<"1. Login\n2. Exit"<<endl;
        cin>>option;
        if (option == 1)
        {
            enterWebBrowserMode(servername, serverport);
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


int enterWebBrowserMode(string servername, string serverport)
{
    int addr_info, sock_fd, sock_conn;
    struct addrinfo *serv_info, *rp;
    string username, pw;

    /* Get server info */
    addr_info = getAddrInfo(servername, serverport, &serv_info);
    if (addr_info != 0)
    {
        printf("Error (getaddrinfo): %s\n", gai_strerror(addr_info));
        return -1;
    }

    /* create a client socket and connect the socket from the list of addrinfo*/
    for (rp = serv_info; rp != NULL; rp = rp->ai_next)
    {
        sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock_fd < 0)
            continue;
        DEBUG("Socket created\n");
        sock_conn = connect(sock_fd, rp->ai_addr, rp->ai_addrlen);
        if (sock_conn != -1)
            break;
        close(sock_fd);
    }
    if (rp == NULL)
    {
        printf("No address in the list was a success\n");
        return -1;
    }
    freeaddrinfo(serv_info);
    DEBUG("Socket connected \n");
    /*Getting username and password from stdin*/
    getLoginInfo(username, pw);

    /*Creating and sending packet*/
    sendPacket(sock_fd, LOGIN, username, pw);

    /* create 2 threads to handle read and write */
    pthread_t th1, th2;
    pthread_attr_t ta;
    (void) pthread_attr_init(&ta);
    (void) pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&th1, &ta, (void * (*)(void *))readThread,(void *)((long)sock_fd)) < 0)
    {
        perror("\nRead Thread create error");
        return -1;
    }
    if (pthread_create(&th2, &ta, (void * (*)(void *))writeThread,(void *)((long)sock_fd)) < 0)
    {
        perror("\nWrite Thread create error");
        return -1;
    }
    return 0;
}


void getLoginInfo(string &username, string &pw)
{
    cout<<"Enter User Name:";
    cin>>username;
    cout<<"\nEnter Password:";
    cin>>pw;
    cout<<endl;
    return;
}

void readThread(int sock_fd)
{
    /*Print the list of commands*/
    int cmd_entered;
    printCmdList();
    while(1)
    {
        cin>> cmd_entered;
        if (cmd_entered == 0)
            printCmdList();
        else if (cmd_entered == 1)
        {
            /*Call list users function*/
            listUsers(sock_fd);
        }
        else if (cmd_entered == 2)
        {
            /*Call post function*/
            post();
        }
        else if (cmd_entered == 3)
        {
        }
        else if (cmd_entered == 4)
        {
        }
        else
        {
            cout<<"Invalid Option. Try again !!!\n";
        }
    }
}

void listUsers(int sock_fd)
{
    sendPacket(sock_fd, LIST, "", "");
    return;
}

void post()
{
    int postwall;
    cout<<"1. Own wall\n";
    cout<<"2. Others wall\n";
    cin>>postwall;
}

void printCmdList()
{
    cout<<"Commands (Enter 0- 3\n"<<"--------------\n";
    cout<<"1. List all users\n";
    cout<<"2. Post to wall\n";
    cout<<"3. Show wall\n";
    cout<<"4. Logout\n";
    cout<<"Enter 0 to print the command list\n";
    return;
}

void writeThread(int sock_fd)
{

}

int sendPacket(int sock_fd, enum commands cmd_code, string key, string value)
{
    struct packet pkt;
    int send_bytes;

    pkt.cmd_code = cmd_code;
    pkt.contents[key] = value;
    pkt.req_num = ++req_num;
    pkt.content_len = key.length() + value.length() + sizeof(enum commands) + sizeof(req_num);

    send_bytes = write(sock_fd, &pkt, sizeof(pkt));
    if (send_bytes < 0)
    {
        perror("Write error\n");
        return -1;
    }
    return 0;
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

