#include <string>
#include <vector>
#include "game.h"

using namespace std;

void game::addPlayer(string _name) {
	player toAdd;
	toAdd.name = _name;
	toAdd.position = 0;
	toAdd.score = 0;
	players.push_back(toAdd);
}

void game::removePlayer(string _name) {
	int index = this->getPlayerIndex(_name);
	if (index != -1) {
		this->players.erase(this->players.begin() + index);
	}
}

int game::getPlayerIndex(string _name) {
	for (int i = 0; i < this->players.size; ++i) {
		if (this->players[i].name == _name) {
			return i;
		}
	}
	return -1;
}

void game::changeScore(int index, int toAdd) {
	this->players[index].score += toAdd;
	if (this->players[index].score >= this->maxScore) {
		this->endGame();
	}
}

void game::endGame() {
	for (int i = 0; i < this->players.size; ++i) {
		this->players[i].score = 0;
	}
}