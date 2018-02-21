#ifndef GAME_H
#define GAME_H

#define SCORE_LIMIT 10
#define MAX_PLAYERS 4

#include <string>
#include <vector>

using namespace std;

struct xyCoords {
	float x;
	float y;
};

struct player {
	string name;
	int ready;
	int color;
	float position;
	int score;
};

class game {
public:
	bool running;
	bool started;
	bool scored;
	bool sendColors;
	vector<player*> players;
	xyCoords ballPos;
	int ballDirection;
	int lastPlayerHit;
	int maxScore;
	
	game() {
		running = false;
		started = false;
		scored = false;
		sendColors = false;
		players = vector<player*>();
		for (int i = 0; i < MAX_PLAYERS; ++i) {
			players.push_back(nullptr);
		}
		ballPos.x = 250;
		ballPos.y = 250;
		ballDirection = rand() % 360;
		maxScore = SCORE_LIMIT;
	}

	void addPlayer(int id, string _name, int color);
	void removePlayer(int id);
	int getNumOfPlayers();
	bool allReady();
	void updateBall();
	void resetBall();
	void changeScore(int index, int toAdd);
	void startGame();
	void stopGame();

private:
	xyCoords lastBallPos;

	void flipBallVertical();
	void flipBallHorizontal();
};

#endif