TEAM MEMBERS -> Contributions
-----------------------------
Tyler Gemora: 31672308

  - Server programmed in C++
    - Implemented server physics
    - Implemented server socket protocol
    - Designed system packet protocol
    - Implemented server depacketization and call assignment protocol
    - Updated server to handle multiple clients
    - Updated client socket protocol
    - Updated server game physics and logic
    - Updated server to implement various artificial latencies
      - Random latency
      - Incremental latency
      - Constant latency
    - Implemented server latency recording
    - Templated milestone report
    - Provided latency mitigation algorithm pseudocode
    - Assisted in debugging and testing client latency mitigation techniques
    - General debugging and testing

Khoa Nguyen : 43498533

  - Client interface programmed in HTML and Javascript
    - Implemented client socket protocol
    - Designed and implemented prototype client interface
    - Implemented client depacketization and call assignment protocol
    - Assisted in upgrading and reintegrating client with server
    - Integrated updated client with updated server
    - Implemented client queuing music
    - Implemented client calculation of latency
    - Updating client game logic and data management
    - Implemented client's side extrapolation and interpolation
    - General debugging and testing

Eisah Jones : 13728047

  - Server and Client programmed in respective languages
    - Assisted in implementing various functions of server and client interface
    - Designed and updated client interface and protocols
    - Reintegrated updated client protocols with the server
    - Updated client interface
    - Integrated updated client with updated server
    - Created HTML framework for displaying graphed data on client
    - Integrated client with graph framework
    - Created GIFs and uploaded them to document
    - Implemented client's side extrapolation and interpolation
    - General debugging and testing
    - Refactored and uploaded assignments

IMPORTANT INFORMATION
---------------------

* Player controls are based on orientation of paddle

* Run the client with the chatroom_client2.0.html file

* Run the server with the main.cpp file

* The bottom message will update with debugging information, please ignore

* Designated server port: 8082

* In order to win the game, the player must score 10 points.

* Upon turning in this project all files compile and the project performs as intended.

* Turn up the volume to listen to those jams

HOW TO RUN DIFERRENT LATENCY MODES:
----------------------------------
* Open file websocket.h, at the top of the file, there is a variable name "LATENCY_MODE".
    
* Initially, LATENCY_MODE will be set to a value ranged between 0 - 3, changing this value will set the server to apply different latency modes.
	+ LATENCY_MODE Values:
	   0 - fixed/constant latency ~ 400ms
	   1 - random latency, uniform. - 0 ~ 800ms.
	   2 - incremental latency - start from 0ms - 800ms, every 100ms.

* Recompile and run the server.
* Go to demo4_client and open chatroom_client2.0 to play.

** Make sure you connect all 4 clients to the game before pressing ready button for any of them.

KNOWN BUGS
----------

# The color of player paddles sometime default to red if packet info is not completely received

# If the client presses ready right after they are connected, sometimes they won't see the other paddles because of an issue where the “opponent” array does not get initialized correctly. Client does not receive information in time.

