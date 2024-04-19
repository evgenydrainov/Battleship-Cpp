#pragma once

#include <raylib.h>

#define GAME_W 320
#define GAME_H 240

#define WIDTH 10
#define HEIGHT 10

struct Game;
extern Game* game;

struct Ship {
	int x;
	int y;
	int width = 1;
	int height = 1;
};

struct Game {
	RenderTexture2D game_texture;

	void init();
	void destroy();
	void run();

	void update(float delta);
	void draw(float delta);
	void draw_ship(Ship& ship);

};
