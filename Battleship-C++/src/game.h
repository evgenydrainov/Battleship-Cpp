#pragma once

#include <raylib.h>

#define GAME_W 320
#define GAME_H 240

#define WIDTH 10
#define HEIGHT 10

#define PLAYER_MAX_SHIPS 10
#define MAX_PLAYERS 2
#define SHIP_MAX_LENGTH 4

struct Game;
extern Game* game;

struct Ship {
	int x;
	int y;
	int width = 1;
	int height = 1;
	bool is_hit[SHIP_MAX_LENGTH][SHIP_MAX_LENGTH];
};

struct Player {
	Ship ships[PLAYER_MAX_SHIPS];
	int ship_count;
};

enum {
	GAME_STATE_PLACING_SHIPS,
	GAME_STATE_PLAYING,
};

struct Game {
	int state;

	Player players[MAX_PLAYERS];
	int player_index;

	int placing_ship_index;
	int placing_ship_w;
	int placing_ship_h;

	RenderTexture2D game_texture;

	void init();
	void destroy();
	void run();

	void update(float delta);
	void draw(float delta);

	Player& get_opponent(int player_index);
	int find_ship(Player& p, int x, int y);

	int get_hovered_cell_x();
	int get_hovered_cell_y();
	Ship get_hovered_ship(int width, int height);
	void draw_ship(Ship& ship);
	void draw_player_ships(Player& p);

};
