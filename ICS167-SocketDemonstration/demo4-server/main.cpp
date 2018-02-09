#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
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
    server.wsSend(clientID, "Welcome!");
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
}

/* called when a client sends a message to the server */
void messageHandler(int clientID, string message)
{
    ostringstream os;
    os << "Stranger " << clientID << " says: " << message;

    vector<int> clientIDs = server.getClientIDs();
	for (int i = 0; i < clientIDs.size(); i++) {
		if (clientIDs[i] != clientID)
			server.wsSend(clientIDs[i], os.str());
	}
	string prefix = message.substr(0, message.find(':'));
	if (prefix == "Name")
	{
		server.addPlayer(clientID, message.substr(message.find(':') + 1));
	}
	else if (prefix == "Position")
	{
		float newPos = stof(message.substr(message.find(':') + 1));
		server.editPlayerPos(clientID, newPos);
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

    /* set event handler */
    server.setOpenHandler(openHandler);
    server.setCloseHandler(closeHandler);
    server.setMessageHandler(messageHandler);
    //server.setPeriodicHandler(periodicHandler);

    /* start the chatroom server, listen to ip '127.0.0.1' and port '8000' */
    server.startServer(PORT1);

    return 1;
}
