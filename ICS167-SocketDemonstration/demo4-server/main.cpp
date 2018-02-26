#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <chrono>

#include "websocket.h"

#define PORT1 8082
using namespace std;

webSocket server;

/* called when a client connects */
void openHandler(int clientID){
    /*ostringstream os;
    os << "Stranger " << clientID << " has joined.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }*/
}

/* called when a client disconnects */
void closeHandler(int clientID){
    /*ostringstream os;
    os << "Stranger " << clientID << " has leaved.";

    vector<int> clientIDs = server.getClientIDs();
    for (int i = 0; i < clientIDs.size(); i++){
        if (clientIDs[i] != clientID)
            server.wsSend(clientIDs[i], os.str());
    }*/
	server.removePlayer(clientID);
	server.sendToAll("Disconnect:" + to_string(clientID));
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message)
{
    /*ostringstream os;
    os << "Stranger " << clientID << " says: " << message;

    vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); i++) {
		if (clientIDs[i] != clientID)
			server.wsSend(clientIDs[i], os.str());
	}*/
	vector<int> clientIDs = server.getClientIDs();
	string prefix = message.substr(0, message.find(':'));
	if (prefix == "Name")
	{
		server.addPlayer(clientID, message.substr(message.find(':') + 1, message.find(';') - message.find(':') - 1), message.substr(message.find(';') + 1));
		ostringstream os;
		os << "ID:" << clientID;
		server.wsSend(clientID, os.str());
	}
	else if (prefix == "Position")
	{
		float newPos = stof(message.substr(message.find(':') + 1));
		server.editPlayerPos(clientID, newPos);
	}
	else if (prefix == "Ready")
	{
		server.readyPlayer(clientID, stoi(message.substr(message.find(':') + 1)));
	}
	else if (prefix == "Time")
	{
		int time = (int)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() % 1000);
		server.wsSend(clientID, server.getTime(time, message.substr(message.find(':') + 1)));
	}
	else if (prefix == "TimeF")
	{
		int time = (int)(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() % 1000);
		server.recordTime(clientID, time, message.substr(message.find(':') + 1));
	}
	else
	{
		printf("Unknown prefix");
	}
}

/* called once per select() loop */
void periodicHandler()
{
    static time_t next = time(NULL) + 10;
    time_t current = time(NULL);
    if (current >= next)
	{
        /*ostringstream os;
		//Deprecated ctime API in Windows 10
		char timecstring[26];
		ctime_s(timecstring, sizeof(timecstring), &current);
		string timestring(timecstring);
        timestring = timestring.substr(0, timestring.size() - 1);
        os << timestring;

        vector<int> clientIDs = server.getClientIDs();
        for (int i = 0; i < clientIDs.size(); i++)
            server.wsSend(clientIDs[i], os.str());*/

        //next = time(NULL) + 1;
		
    }

	
}

int main(int argc, char *argv[]){
	//std::cout << to_string(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(PORT1);

    return 1;
}