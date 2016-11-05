#pragma once
#include "SDL.h"
#include <iostream>

using namespace std;

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window * window = SDL_CreateWindow("SNEK",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 640, SDL_WINDOW_SHOWN);
	SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STATIC, 640, 640);

	SDL_Surface * image = SDL_LoadBMP("assets/lena_gray.bmp");
	SDL_Texture * imageTexture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);

	for (int i = 0; i < 2; ++i) {
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, imageTexture, NULL, NULL);
		SDL_RenderPresent(renderer);
		SDL_Delay(1000);
	}

	SDL_DestroyTexture(imageTexture);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
