#pragma once

#include <raylib.h>

#include <fstream>
#include <iostream>

#define GAME_W 320
#define GAME_H 240

#define WIDTH 10
#define HEIGHT 10

#define PLAYER_MAX_SHIPS 10
#define MAX_PLAYERS 2

enum {
	CELL_IS_SHIP = 1,
	CELL_IS_HIT = 1 << 1,
};

struct Game;
extern Game* game;

struct Player {
	unsigned char board[HEIGHT][WIDTH];
};

enum {
	GAME_STATE_PLACING_SHIPS,
	GAME_STATE_PLAYING,
	GAME_STATE_WON,
};

struct Game {
	int state;

	Player players[MAX_PLAYERS];
	int player_index;

	int placing_ship_index;
	int placing_ship_x;
	int placing_ship_y;
	int placing_ship_w;
	int placing_ship_h;

	int board_xoff = 32;
	int board_yoff = 32;

	const char* message = "";
	float wait_timer = 0;

	RenderTexture2D game_texture;

	std::ofstream out;
	std::streambuf *coutbuf;

	void init();
	void destroy();
	void run();

	void update(float delta);
	void draw(float delta);

	Player& get_opponent(int player_index);

	bool can_place_ship(Player& p, int ship_x, int ship_y, int ship_w, int ship_h);
	int get_hovered_cell_x();
	int get_hovered_cell_y();
	void draw_player_board(Player& p, bool show_ships = true);

};
