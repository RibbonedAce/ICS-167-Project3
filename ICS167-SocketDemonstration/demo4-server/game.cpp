#define _USE_MATH_DEFINES

#define X_BOUNDS 550
#define Y_BOUNDS 550
#define BALL_RADIUS 10
#define PADDLE_OFFSET 20
#define PADDLE_LENGTH 120
#define PADDLE_WIDTH 10

#include <math.h>
#include <string>
#include <time.h>
#include <vector>
#include "game.h"

using namespace std;

void game::addPlayer(int id, string _name, int color) {
	player toAdd;
	toAdd.name = _name;
	toAdd.ready = 0;
	toAdd.color = color;
	toAdd.position = (X_BOUNDS - PADDLE_LENGTH) / 2;
	toAdd.score = 0;
	removePlayer(id);
	players[id] = new player(toAdd);
}

void game::removePlayer(int id) {
	if (players[id] != nullptr) {
		delete players[id];
		players[id] = nullptr;
	}
	stopGame();
}

int game::getNumOfPlayers() {
	int result = 0;
	for (const auto& i : players) {
		if (i != nullptr) {
			++result;
		}
	}
	return result;
}

bool game::allReady() {
	for (const auto& i : players) {
		if (i == nullptr || i->ready <= 0) {
			return false;
		}
	}
	return true;
}

void game::updateBall() {
	if (!running && allReady()) {
		startGame();
	}
	clock_t currentTime = clock();
	if (running) {
		lastBallPos.x = ballPos.x;
		lastBallPos.y = ballPos.y;
		ballPos.x += (float)(currentTime - lastTime) / CLOCKS_PER_SEC * 60 * cos(ballDirection * M_PI / 180);
		ballPos.y += (float)(currentTime - lastTime) / CLOCKS_PER_SEC * 60 * sin(ballDirection * M_PI / 180);
		if (ballPos.y + BALL_RADIUS > Y_BOUNDS || ballPos.y - BALL_RADIUS < 0 ||
			ballPos.x + BALL_RADIUS > X_BOUNDS || ballPos.x - BALL_RADIUS < 0) {
			changeScore(lastPlayerHit, 1);
			resetBall();
		}
		if (ballPos.x - BALL_RADIUS < PADDLE_OFFSET + PADDLE_WIDTH && 
			lastBallPos.x - BALL_RADIUS >= PADDLE_OFFSET + PADDLE_WIDTH && 
			ballPos.y - players[0]->position > 0 && ballPos.y - players[0]->position < PADDLE_LENGTH) {
			flipBallHorizontal();
			lastPlayerHit = 0;
		}
		if (ballPos.y - BALL_RADIUS < PADDLE_OFFSET + PADDLE_WIDTH &&
			lastBallPos.y - BALL_RADIUS >= PADDLE_OFFSET + PADDLE_WIDTH &&
			ballPos.x - players[1]->position > 0 && ballPos.x - players[1]->position < PADDLE_LENGTH) {
			flipBallVertical();
			lastPlayerHit = 1;
		}
		if (ballPos.x + BALL_RADIUS > X_BOUNDS - (PADDLE_OFFSET + PADDLE_WIDTH) &&
			lastBallPos.x + BALL_RADIUS <= X_BOUNDS - (PADDLE_OFFSET + PADDLE_WIDTH) &&
			ballPos.y - players[2]->position > 0 && ballPos.y - players[2]->position < PADDLE_LENGTH) {
			flipBallHorizontal();
			lastPlayerHit = 2;
		}
		if (ballPos.y + BALL_RADIUS > Y_BOUNDS - (PADDLE_OFFSET + PADDLE_WIDTH) &&
			lastBallPos.y + BALL_RADIUS <= Y_BOUNDS - (PADDLE_OFFSET + PADDLE_WIDTH) &&
			ballPos.x - players[3]->position > 0 && ballPos.x - players[3]->position < PADDLE_LENGTH) {
			flipBallVertical();
			lastPlayerHit = 3;
		}
	}
	lastTime = currentTime;
}

void game::resetBall() {
	ballPos.x = X_BOUNDS / 2;
	ballPos.y = Y_BOUNDS / 2;
	ballDirection = rand() % 360;
	lastPlayerHit = -1;
}

void game::changeScore(int index, int toAdd) {
	if (index >= 0 && players[index] != nullptr) {
		scored = true;
		players[index]->score += toAdd;
		if (players[index]->score >= maxScore) {
			stopGame();
		}
	}
}

void game::startGame() {
	resetBall();
	sendColors = true;
	started = true;
	running = true;
}

void game::stopGame() {
	for (int i = 0; i < players.size(); ++i) {
		if (players[i] != nullptr) {
			players[i]->score = 0;
		}
	}
	running = false;
}

void game::flipBallVertical() {
	ballDirection = (ballDirection * -1) % 360;
}

void game::flipBallHorizontal() {
	ballDirection = (180 - ballDirection) % 360;
}