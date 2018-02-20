//
// Created by Eisah Jones on 2/4/18.
//

#ifndef PONGSERVER_CLIENT_H
#define PONGSERVER_CLIENT_H

#include <cstdlib>
#include <cstring>
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <cstdio>
//#include <sys/socket.h>
//#include <arpa/inet.h>
#include <zconf.h>
#include <thread>
#include <vector>
#include <iostream>
#include <thread>

class client
{
public:
    //Constructors
    client( void );
    client(SOCKET s, int i);

    //Accessors
    std::string getName( void );

    //Mutators
    void incrementScore( void );
    void resetScore( void );
    void setPaddlePosition(int x, int y);
    void setName(std::string s);


    //Networking
    void receivePackets( void );
    void sendPacket(const char* p);
    void initializeConnection( void );
    void processPacket ( char p[] );
    void disconnect ( void );

private:

    WSADATA wsaData;
    int resultNum;
    struct addrinfo hints;
    struct addrinfo* result;

    in_addr addr;
    u_short port;
    SOCKET clientSocket;
    int clientScore = 0;
    int clientPlace = 0;
    std::string _clientName;
    std::pair<int, int> paddlePosition;
    bool exists = true;
    bool ready = false;

    void _packetize(std::string s);

};


#endif //PONGSERVER_CLIENT_H
