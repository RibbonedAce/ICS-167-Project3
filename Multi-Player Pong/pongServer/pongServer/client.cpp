//
// Created by Eisah Jones on 2/4/18.
//

#include "client.h"

client::client( void )
{
    _clientName = "NULL";
}

client::client( SOCKET s, int i)
{
    clientSocket = s;
    _clientName = "Player" + std::to_string(i);
    clientPlace = i;
    return;
}

std::string client::getName( void ){
    return _clientName;
}

void client::incrementScore( void ){
    clientScore++;
}

void client::resetScore( void ){
    clientScore = 0;
}

void client::setPaddlePosition(int x, int y){
    std::pair<int, int> paddlePosition(x, y);
}
void client::setName(std::string s){
    _clientName = s;
}

void client::receivePackets( void )
{
    while (exists)
    {
        char clientMessage[1024];
        if (recv(clientSocket, clientMessage, 256, 0) < 0)
            std::cout << "Error receiving packet: " << WSAGetLastError() << std::endl;
        else
            std::cout << _clientName << ": " << clientMessage << std::endl;
    }
    return;
}

void client::sendPacket(const char* p)
{
    ssize_t error = send(clientSocket, p, sizeof(p), 0);
    if (error == -1){
        std::cout << "Error sending packet: " << p << std::endl;
    }
    return;
}

void client::initializeConnection( void )
{
    std::string m = "connect: You have connected to the server!";
    sendPacket(m.c_str());

    //Start thread for receiving information
    while (exists)
    {
        char clientMessage[256];
        if (recv(clientSocket, clientMessage, 256, 0) < 0)
            std::cout << "Error receiving packet: " << WSAGetLastError() << std::endl;
        else {
            std::cout << _clientName << ": " << clientMessage << std::endl;
            //start thread that processes task
            processPacket(clientMessage);

        }
    }
}

void client::processPacket ( char p[] )
{
    for (auto c : p){

    }
    return;
}

void client::disconnect ( void )
{
    return;
}

void client::_packetize(std::string s)
{
    return;
}