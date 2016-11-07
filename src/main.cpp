#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

static const int SCREEN_HEIGHT = 640;
static const int SCREEN_WIDTH = 640;
static const int BLOCK_SIZE = 10;
static const int DEFAULT_TIME_PER_GAME_TICK = 250;
static const float DEFAULT_TIME_MULTIPLIER = 0.9;
using namespace std;

struct Color {
	Uint8 r,g,b,a;
};

struct SnakeSection {
	int x, y;
	SnakeSection(int inX, int inY) : x(inX), y(inY) {};
};

struct Snake {
private:

	void moveHeadTowardsCurrentDirection() {
		switch (currentDirection) {
		case 'l':
			snakeSections.front().x -= BLOCK_SIZE;
			break;
		case 'r':
			snakeSections.front().x += BLOCK_SIZE;
			break;
		case 'u':
			snakeSections.front().y -= BLOCK_SIZE;
			break;
		case 'd':
			snakeSections.front().y += BLOCK_SIZE;
			break;
		default:
			break;
		}
	}
	void moveTailSectionsTowardsHead() {
		for (auto snakeSection = snakeSections.rbegin(); (snakeSection + 1) != snakeSections.rend(); ++snakeSection) {
			SnakeSection sectionAheadOfCurrent = *(snakeSection + 1);
			snakeSection->x = sectionAheadOfCurrent.x;
			snakeSection->y = sectionAheadOfCurrent.y;
		}
	}
public:
	int numberSections;
	char currentDirection;
	vector<SnakeSection> snakeSections;
public:
	Snake(int initialX, int initialY, char direction) : currentDirection(direction), numberSections(1) {
		snakeSections.push_back(SnakeSection(initialX, initialY));
	};

	//TODO: Make it so you can't collide into yourself. so you cannot invert your direction. 
	void setDirection(char direction) {
		switch (direction) {
		case 'l':
			if (this->currentDirection == 'r') {
				return;
			}
			break;
		case 'r':
			if (this->currentDirection == 'l') {
				return;
			}
			break;
		case 'u':
			if (this->currentDirection == 'd') {
				return;
			}
			break;
		case 'd':
			if (this->currentDirection == 'u') {
				return;
			}
			break;
		}
		this->currentDirection = direction;

	};

	
	void move() {
		moveTailSectionsTowardsHead();
		moveHeadTowardsCurrentDirection();
	};

	void addSection() {
		numberSections++;
		SnakeSection endSection = snakeSections.back();
		snakeSections.push_back(endSection);
	};
};

struct Food {
	int x, y;
	Food() : Food(100, 100) {};
	Food(int inX, int inY) : x(inX), y(inY) {};
	void setPosition(int inX, int inY) {
		this->x = inX;
		this->y = inY;
	};
};

class GameGraphicsDelegate {
private:
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	TTF_Font * font;
private:
	void drawRectangle(int x, int y, Color color, int width = 10, int height = 10) {
		SDL_Rect rectangle = { x,y,width,height };
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rectangle);
	};

	void drawBorders() {
		Color borderColor = { 0xFF, 0x00, 0x00, 0xFF };
		drawRectangle(0, 0, borderColor, 640, 10);
		drawRectangle(0, 630, borderColor, 640, 10);
		drawRectangle(0, 0, borderColor, 10, 640);
		drawRectangle(630, 0, borderColor, 10, 640);
	};

	void drawFood(const Food &food) {
		Color borderColor = { 0xFF, 0xFF, 0x00, 0xFF };

		drawRectangle(food.x, food.y, borderColor);
	};

	void drawSnake(const Snake &snake) {
		for (auto snakePart = snake.snakeSections.begin(); snakePart != snake.snakeSections.end(); ++snakePart) {
			Color borderColor = { 0x00, 0xFF, 0x00, 0xFF };

			drawRectangle(snakePart->x, snakePart->y, borderColor);
		}
	};

	void drawUI(const int &score) {
		std::string text = "score: " + to_string(score);
		SDL_Color textColor = { 0xFF, 0xFF, 0x00, 0xFF };
		SDL_Surface * fontSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
		SDL_Texture * fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
		SDL_Rect textLocation = { SCREEN_HEIGHT - 100 - BLOCK_SIZE, BLOCK_SIZE, 100, 50 };
		SDL_RenderCopy(renderer, fontTexture, NULL, &textLocation);
		SDL_FreeSurface(fontSurface);
		SDL_DestroyTexture(fontTexture);
	}
public:
	void init() {
		window = SDL_CreateWindow("SNEK",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
		font = TTF_OpenFont("assets/Fantasque.ttf", 28);
	};

	void drawGame(const int &score, const Snake &snake, const Food &food) {
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		drawBorders();
		drawSnake(snake);
		drawFood(food);
		drawUI(score);
		SDL_RenderPresent(renderer);
	};

	void terminate() {
		TTF_CloseFont(font);
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	};
};

class Game {
private:
	GameGraphicsDelegate gameGraphicsDelegate;
	Snake snake;
	Food food;
public:
	int score = 0;
private:

	bool snakeCollidesWithFood() {
		SnakeSection snakeHead = snake.snakeSections.front();
		bool sameX = (food.x == snakeHead.x);
		bool sameY = (food.y == snakeHead.y);
		return  sameX && sameY;
	};

	bool snakeCollidesWithSelf() {
		SnakeSection snakeHead = snake.snakeSections.front();
		for (auto snakeSection = (snake.snakeSections.begin() + 1); snakeSection != snake.snakeSections.end(); ++snakeSection) {
			if (snakeHead.x == snakeSection->x && snakeHead.y == snakeSection->y) {
				return true;
			}
		}
		return false;
	};

	bool snakeCollidesWithBarrier() {
		SnakeSection snakeHead = snake.snakeSections.front();
		bool invalidX = (snakeHead.x <= BLOCK_SIZE || snakeHead.x >= SCREEN_WIDTH - BLOCK_SIZE);
		bool invalidY = (snakeHead.y <= BLOCK_SIZE || snakeHead.y >= SCREEN_HEIGHT - BLOCK_SIZE);
		return invalidX || invalidY;
	};

	bool snakeCollidesWithIllegalTerrain() {
		return snakeCollidesWithSelf() || snakeCollidesWithBarrier();
	}

	void updateFromPointGain() {
		score++;
		snake.addSection();
		addFood();
	}

	void generateRandomXY(int &x, int &y) {
		x = rand() % (SCREEN_WIDTH - BLOCK_SIZE*2) + BLOCK_SIZE;
		x = x - (x % BLOCK_SIZE);
		y = rand() % (SCREEN_HEIGHT - BLOCK_SIZE*2) + BLOCK_SIZE;
		y = y - (y % BLOCK_SIZE);
	}
	void addFood() {
		int newX, newY;
		generateRandomXY(newX, newY);
		bool invalidPosition = true;
		while (invalidPosition) {
			invalidPosition = false;
			for (auto snakeSection = snake.snakeSections.begin(); snakeSection != snake.snakeSections.end(); ++snakeSection) {
				if (newX == snakeSection->x && newY == snakeSection->y) {
					invalidPosition = true;
					generateRandomXY(newX, newY);
				}
			}
		}
		
		food.setPosition(newX, newY );
	};
public:
	Game() : snake(400,400,'l') {};
	
	void handleKeyPress(const SDL_Event event) {
		switch (event.key.keysym.sym) {
		case SDLK_a:
			snake.setDirection('l');
			break;
		case SDLK_s:
			snake.setDirection('d');
			break;
		case SDLK_d:
			snake.setDirection('r');
			break;
		case SDLK_w:
			snake.setDirection('u');
			break;
		case SDLK_r:
			reset();
			break;
		}
	};

	void drawGame() {
		gameGraphicsDelegate.drawGame(score, snake, food);
	};

	void start() {
		gameGraphicsDelegate.init();
		srand(0);
		addFood();
	};

	void runState() {
		if (snakeCollidesWithIllegalTerrain()) {
			reset();
		} else if (snakeCollidesWithFood()) {
			updateFromPointGain();
		}
		snake.move();	
	};

	void reset() {
		score = 0;
		snake = Snake(400, 400, 'l');
		addFood();
	};

	void terminate() {
		gameGraphicsDelegate.terminate();
	};


};

void init() {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
};

void terminate() {
	TTF_Quit();
	SDL_Quit();
};

bool handleKeyEvents(Game &game) {
	SDL_Event event;

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			return true;
		} else if (event.type == SDL_KEYDOWN) {
			game.handleKeyPress(event);
		}
	}
	return false;
};

void updateGameTick(const Game &game,int &score, Uint32 &timePerGameTick) {
	if (game.score == 0) {
		score = 0;
		timePerGameTick = DEFAULT_TIME_PER_GAME_TICK;
	} else if (score < game.score) {
		score++;
		timePerGameTick = (Uint32)(DEFAULT_TIME_MULTIPLIER*timePerGameTick);
	}
};

inline void updateTimers(Uint32 &startTick, Uint32 &timeTillNextGameTick) {
	Uint32 currentTick = SDL_GetTicks();
	timeTillNextGameTick += currentTick - startTick;
	startTick = currentTick;
};

void runLoop(Game &game) {
	Uint32 startTick = SDL_GetTicks();
	Uint32 timeTillNextGameTick = 0;
	Uint32 timePerGameTick = DEFAULT_TIME_PER_GAME_TICK;
	int score = game.score;
	while (true) {
		if (handleKeyEvents(game)) {
			break;
		}

		updateTimers(startTick, timeTillNextGameTick);
		while (timeTillNextGameTick > timePerGameTick) {
			timeTillNextGameTick -= timePerGameTick;
			game.runState();
			updateGameTick(game, score, timePerGameTick);
		}
		game.drawGame();
	}
};

int main(int argc, char* args[]) {
	init();
	Game game;
	game.start();
	runLoop(game);
	game.terminate();
	terminate();

	return 0;
}
