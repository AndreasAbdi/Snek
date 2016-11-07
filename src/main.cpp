#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

static int SCREEN_HEIGHT = 640;
static int SCREEN_WIDTH = 640;
static int BLOCK_SIZE = 10;

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

class Game {
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	TTF_Font * font;
	int score = 0;
	Snake snake;
	Food food;
private:
	void drawRectangle(int x, int y, Color color, int width = 10, int height = 10) {
		SDL_Rect rectangle = { x,y,width,height };
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderFillRect(renderer, &rectangle);
	};

	void drawBorders() {
		Color borderColor = { 0xFF, 0x00, 0x00, 0xFF };
		drawRectangle(0, 0, borderColor, 640, 10);
		drawRectangle(0, 630, borderColor, 640,10);
		drawRectangle(0, 0, borderColor, 10, 640);
		drawRectangle(630, 0, borderColor, 10, 640);
	};

	void drawFood() {
		Color borderColor = { 0xFF, 0xFF, 0x00, 0xFF };

		drawRectangle(food.x, food.y, borderColor);
	};

	void drawSnake() {
		for (auto snakePart = snake.snakeSections.begin(); snakePart != snake.snakeSections.end(); ++snakePart) {
			Color borderColor = { 0x00, 0xFF, 0x00, 0xFF };

			drawRectangle(snakePart->x, snakePart->y, borderColor);
		}
	};

	void drawUI() {
		std::string text = "score: " + to_string(score);
		SDL_Color textColor = { 0xFF, 0xFF, 0x00, 0xFF };
		SDL_Surface * fontSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
		SDL_Texture * fontTexture = SDL_CreateTextureFromSurface(renderer, fontSurface);
		SDL_Rect textLocation = { SCREEN_HEIGHT-100-BLOCK_SIZE, BLOCK_SIZE, 100, 50 };
		SDL_RenderCopy(renderer, fontTexture, NULL, &textLocation);
		SDL_FreeSurface(fontSurface);
		SDL_DestroyTexture(fontTexture);
	}

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

	void addFood() {
		int newX = rand() % (SCREEN_WIDTH - 20) + 10;
		newX = newX - (newX % 10);
		int newY = rand() % (SCREEN_HEIGHT - 20) + 10;
		newY = newY - (newY % 10);
		food.setPosition(newX, newY );
	};
public:
	Game() : snake(400,400,'l') {
		window = SDL_CreateWindow("SNEK",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
		font = TTF_OpenFont("assets/Fantasque.ttf", 28);

	};
	
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
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		drawBorders();
		drawSnake();
		drawFood();
		drawUI();
		SDL_RenderPresent(renderer);
	};

	void start() {
		srand(0);
		food.setPosition(300, 300);
	};

	void runState() {
		if (snakeCollidesWithIllegalTerrain()) {
			reset();
		} else if (snakeCollidesWithFood()) {
			updateFromPointGain();
		}
		snake.move();	
		drawGame();
	};

	void reset() {
		score = 0;
		snake = Snake(400, 400, 'l');
		food.setPosition(300, 300);
	};

	void terminate() {
		TTF_CloseFont(font);	
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	};


};

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	Game game;
	game.start();

	bool quit = false;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			} else if (event.type == SDL_KEYDOWN) {
				game.handleKeyPress(event);
			}
		}
		game.runState();
		SDL_Delay(250);
	}

	game.terminate();
	TTF_Quit();
	SDL_Quit();

	return 0;
}
