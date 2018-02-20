//
// Created by Eisah Jones on 2/4/18.
//


#include "pongServer.h"

#define DEFAULT_PORT "27000"

#pragma comment (lib, "Ws2-32.lib")

pongServer::pongServer()
{
    listeningSocket = INVALID_SOCKET;
    requestSocket = INVALID_SOCKET;

    addrinfo* result = NULL;

    recvbuflen = 512;

    listeningPort = 6000;
    for (int i = 0; i < 4; i++){
        clientArray[i] = client();
    }

}

int pongServer::start() {
    //Initialize Winsock
    resultNum = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (resultNum != 0) {
        printf("WSAStartup failed with error: %d\n", resultNum);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Resolve the server address and port
    resultNum = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (resultNum != 0) {
        printf("getaddrinfo failed with error: %d\n", resultNum);
        WSACleanup();
        return 1;
    }

    //Create a SOCKET for connecting to server
    listeningSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listeningSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }


    //Setup the TCP listening socket
    resultNum = bind(listeningSocket, result->ai_addr, (int) result->ai_addrlen);
    if (resultNum == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    //Listen on the socket
    resultNum = listen(listeningSocket, SOMAXCONN);
    if (resultNum == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listeningSocket);
        WSACleanup();
        return 1;
    }

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(listeningSocket, (struct sockaddr *)&sin, &len) == -1)
        perror("getsockname");
    else
        printf("port number %d\n", ntohs(sin.sin_port));

//    while (gamerunning) {
//        if (Connect != NULL) {
//            SOCKET *client_sock = new SOCKET(accept(Listen, (SOCKADDR*)&Server, &size));
//            if((*client_sock) != INVALID_SOCKET)
//                _beginthread( ClientThread, 0, client_sock);

    int firstSlot;
    while (true) {
        //Accept a client socket
        //SOCKADDR_IN client_info;
        struct sockaddr_in client_info = {0};
        int clientSize = sizeof(client_info);
        requestSocket = accept(listeningSocket, (struct sockaddr*)& client_info, &clientSize);
        if (requestSocket == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(listeningSocket);
            WSACleanup();
            return 1;
        } else {
            char* clientIP = inet_ntoa(client_info.sin_addr);
            int clientPort = htons(client_info.sin_port);
            firstSlot = findFirstSlot();
            if (firstSlot != -1) {
                client newClient(requestSocket, firstSlot + 1);
                clientArray[firstSlot] = newClient;
                printf("received a connection from: %s on port %d\n",
                       clientIP, clientPort);
                std::thread clientThread (&client::initializeConnection, newClient);
                clientThread.detach();
                //printClients();
            } else {
                printf("received a connection from: %s port %d\n",
                       clientIP, clientPort);
                //notify that server is full
                std::cout << "Server is Full" << std::endl;
                std::string message = "serverFullError: The server is full\n";
                send(requestSocket, message.c_str(), message.size(), 0);
                shutdown(requestSocket, 2);    /* close the connection */
            }
        }
    }
}


int pongServer::findFirstSlot() {
    int i = 0;
    for (i; i < 4; i++) {
        if (clientArray[i].getName() == "NULL") {
            break;
        }
    }
    if (i == 3 && clientArray[i].getName() != "NULL")
        return -1;
    return i;
}

void pongServer::printClients() {
    for (int i = 0; i < 4; i++) {
        if (clientArray[i].getName() != "NULL") {
            std::cout << i << ": " << clientArray[i].getName() << std::endl;
        }
    }
    std::cout << std::endl;
    return;
}

int pongServer::sendMessage(SOCKET s, char *message, int messageSize) {
    return send(s, message, messageSize, 0);
}

int pongServer::receiveMessage(SOCKET s, char *buffer, int bufSize) {
    return recv(s, buffer, bufSize, 0);
}
