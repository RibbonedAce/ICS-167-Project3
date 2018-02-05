#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

using namespace std;

struct xyCoords {
	float x;
	float y;
};

struct player {
	string name;
	float position;
	int score;
};

class game {
public:
	vector<player> players;
	xyCoords ballPos;
	int maxScore;
	
	game(int _maxScore) {
		players = vector<player>();
		maxScore = _maxScore;
	}

	void addPlayer(string _name);
	void removePlayer(string _name);
	int getPlayerIndex(string _name);
	void changeScore(int index, int toAdd);
	void endGame();
};

#endif