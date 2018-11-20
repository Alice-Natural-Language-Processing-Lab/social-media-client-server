# CMPE207-Project
CMPE207 Project Repository

Execute the client and server side using these commands
Server
------
g++ -std=c++11 *.cpp -o server -pthread -lmysqlcppconn

./server

Client
------
g++ -std=c++11 *.cpp -o client -pthread

./client


Current Status - server
-----------------------
1. Incorporated create server socket and and accept from the networking library
2. Implemented process each client,packet parsing, session validity check and request processing functionalities
2. server will create each thread for each client and a single thread for notification
3. server will call the APIs of database to fetch information

TODO:
1. Interfacing of notification thread
2. Checking the database interfacing
3. checking the ack
4. Incorporating the read/write from the networking library

Current Status - Client
-----------------------
1. Implemented read and write threads
2. Implemented user interfacing
3. Implemented password hashing
4. Incorporated socket creation from the networking library

TODO:
1. Need to incorporate read/write from networking library
