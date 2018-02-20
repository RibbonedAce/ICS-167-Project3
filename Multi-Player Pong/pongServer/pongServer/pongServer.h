//
// Created by Eisah Jones on 2/4/18.
//

#ifndef PONGSERVER_PONGSERVER_H
#define PONGSERVER_PONGSERVER_H

#include <cstdlib>
#include <cstring>
#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <cstdio>
//#include <arpa/inet.h>
#include <zconf.h>
#include <thread>
#include <vector>
#include "client.h"

#pragma comment (lib, "Ws2-32.lib")



//#define DEFAULT_BUFLEN 512
//#define DEFAULT_PORT "6881"

class pongServer
{
public:
    //Constructors
    pongServer( void );

    //Methods
    int start( void );
    int findFirstSlot( void );

    //Helpers
    void printClients( void );

    //Networking
    int sendMessage(SOCKET s, char* message, int messageSize);
    int receiveMessage(SOCKET s, char* buffer, int bufSize);

private:
    WSADATA wsaData;

    SOCKET listeningSocket;
    SOCKET requestSocket;

    struct addrinfo* result;
    struct addrinfo hints;
    struct addrinfo* joiningClient;

    struct sockaddr* jc;

    int resultNum;
    int sendResult;
    char recvbuf[512];
    int recvbuflen = 512;


    client clientArray[4];


    int listeningPort;


};


#endif //PONGSERVER_PONGSERVER_H
