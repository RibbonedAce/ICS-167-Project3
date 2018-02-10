#define _USE_MATH_DEFINES

#define MAX_PLAYERS 1
#define X_BOUNDS 550
#define Y_BOUNDS 550
#define BALL_RADIUS 10
#define PADDLE_OFFSET 20
#define PADDLE_LENGTH 120
#define PADDLE_WIDTH 10

#include <map>
#include <math.h>
#include <string>
#include <vector>
#include "game.h"

using namespace std;

void game::addPlayer(int id, string _name) {
	player toAdd;
	toAdd.name = _name;
	toAdd.position = 250;
	toAdd.score = 0;
	players[id] = toAdd;
	if (this->getNumOfPlayers() >= MAX_PLAYERS) {
		this->startGame();
	}
}

void game::removePlayer(int id) {
	this->players[id].name = "";
	this->players[id].score = 0;
	this->players[id].position = 250;
	this->stopGame();
}

int game::getNumOfPlayers() {
	int result = 0;
	for (map<int, player>::iterator it = this->players.begin(); it != this->players.end(); ++it) {
		if (it->second.name != "") {
			++result;
		}
	}
	return result;
}

void game::updateBall() {
	if (this->running) {
		this->lastBallPos.x = ballPos.x;
		this->lastBallPos.y = ballPos.y;
		this->ballPos.x += cos(this->ballDirection * M_PI / 180);
		this->ballPos.y += sin(this->ballDirection * M_PI / 180);
		if (this->ballPos.y + BALL_RADIUS > Y_BOUNDS || this->ballPos.y - BALL_RADIUS < 0) {
			flipBallVertical();
		}
		if (this->ballPos.x + BALL_RADIUS > X_BOUNDS) {
			flipBallHorizontal();
		}
		else if (this->ballPos.x - BALL_RADIUS < PADDLE_OFFSET + PADDLE_WIDTH && this->lastBallPos.x - BALL_RADIUS >= PADDLE_OFFSET + PADDLE_WIDTH) {
			if (this->ballPos.y - players[0].position > 0 && this->ballPos.y - players[0].position < PADDLE_LENGTH) {
				changeScore(0, 1);
				flipBallHorizontal();
			}
		}
		else if (this->ballPos.x - BALL_RADIUS < 0) {
			stopGame();
		}
	}
}

void game::changeScore(int index, int toAdd) {
	this->players[index].score += toAdd;
	if (this->players[index].score >= this->maxScore) {
		this->stopGame();
	}
}

void game::startGame() {
	this->running = true;
}

void game::stopGame() {
	for (map<int,player>::iterator it = this->players.begin(); it != this->players.end(); ++it) {
		it->second.score = 0;
	}
	ballPos.x = 250;
	ballPos.y = 250;
	this->running = false;
}

void game::flipBallVertical() {
	ballDirection = (ballDirection * -1) % 360;
}

void game::flipBallHorizontal() {
	ballDirection = (180 - ballDirection) % 360;
}