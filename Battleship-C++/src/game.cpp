#include "game.h"

#include "util.h"
#include "mathh.h"
#include <rlgl.h>
#include <raymath.h>

Game* game;

static int ship_lengths_to_place[10] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};

void Game::init() {
	InitWindow(3 * GAME_W, 3 * GAME_H, "Battleship-C++");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

	rlDisableBackfaceCulling();

	game_texture = LoadRenderTextureNoAlphaNoDepth(GAME_W, GAME_H);

	state = GAME_STATE_PLACING_SHIPS;
	player_index = 0;
	placing_ship_w = 1;
	placing_ship_h = ship_lengths_to_place[placing_ship_index];
}

void Game::destroy() {
	UnloadRenderTexture(game_texture);

	CloseWindow();
}

void Game::run() {
	while (!WindowShouldClose()) {
		float delta = 1;

		update(1);
		draw(1);
	}
}

void Game::update(float delta) {
	switch (state) {
		case GAME_STATE_PLACING_SHIPS: {
			if (IsKeyPressed(KEY_R)) {
				placing_ship_index = 0;

				placing_ship_w = 1;
				placing_ship_h = ship_lengths_to_place[placing_ship_index];

				Player& p = players[player_index];
				p.ship_count = 0;
			}

			if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_SPACE)) {
				int temp = placing_ship_w;
				placing_ship_w = placing_ship_h;
				placing_ship_h = temp;
			}

			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				Player& p = players[player_index];
				int ship_index = p.ship_count;

				Ship ship = get_hovered_ship(placing_ship_w, placing_ship_h);

				p.ships[ship_index] = ship;
				p.ship_count++;

				placing_ship_index++;

				if (placing_ship_index >= ArrayLength(ship_lengths_to_place)) {
					player_index++;
					placing_ship_index = 0;

					if (player_index >= MAX_PLAYERS) {
						state = GAME_STATE_PLAYING;
						player_index = 0;
					}
				}

				placing_ship_w = 1;
				placing_ship_h = ship_lengths_to_place[placing_ship_index];
			}
			break;
		}

		case GAME_STATE_PLAYING: {
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_SPACE)) {
				int x = get_hovered_cell_x();
				int y = get_hovered_cell_y();

				Player& p = players[player_index];
				if (!p.has_shot[y][x]) {
					Player& opponent = get_opponent(player_index);
					int ship_index = find_ship(opponent, x, y);

					if (ship_index != -1) {
						Ship& ship = opponent.ships[ship_index];
						int rel_x = x - ship.x;
						int rel_y = y - ship.y;
						if (!ship.is_hit[rel_y][rel_x]) {
							ship.is_hit[rel_y][rel_x] = true;
							p.has_shot[y][x] = true;

							state = GAME_STATE_PLAYING;
							player_index++;
							player_index %= MAX_PLAYERS;
						}
					}
				}
			}
			break;
		}
	}
}

void Game::draw(float delta) {
	BeginDrawing();

	BeginTextureMode(game_texture);
	ClearBackground({100, 149, 237, 255});

	{
		Matrix proj = MatrixOrtho(0, GAME_W, 0, GAME_H, -1, 1);
		rlSetMatrixProjection(proj);

		Matrix modelview = MatrixTranslate(board_xoff, board_yoff, 0);
		rlSetMatrixModelview(modelview);
	}

	for (int y = 0; y < HEIGHT + 1; y++) {
		for (int x = 0; x < WIDTH + 1; x++) {
			DrawRectangleLines(16 * (x - 1), 16 * (y - 1), 17, 17, WHITE);
		}
	}

	for (int x = 0; x < WIDTH; x++) {
		char buf[] = {'A' + x, 0};
		DrawText(buf, 16 * x + 6, 4 - 16, 10, WHITE);
	}

	for (int y = 0; y < HEIGHT; y++) {
		DrawText(TextFormat("%d", y + 1), 4 - 16, 16 * y + 4, 10, WHITE);
	}

	switch (state) {
		case GAME_STATE_PLACING_SHIPS: {
			Player& p = players[player_index];
			draw_player_ships(p);

			Ship ship = get_hovered_ship(placing_ship_w, placing_ship_h);
			draw_ship(ship);
			break;
		}

		case GAME_STATE_PLAYING: {
			if (IsKeyDown(KEY_SPACE)) {
				Player& p = players[player_index];
				draw_player_ships(p);
			} else {
				int x = get_hovered_cell_x();
				int y = get_hovered_cell_y();

				DrawCross(16 * x, 16 * y, 16, 16, RED);
			}
			break;
		}
	}

	EndTextureMode();

	ClearBackground(BLACK);

	{
		Matrix proj = MatrixOrtho(0, GAME_W, GAME_H, 0, -1, 1);
		rlSetMatrixProjection(proj);

		Matrix modelview = MatrixIdentity();
		rlSetMatrixModelview(modelview);
	}

	DrawTexture(game_texture.texture, 0, 0, WHITE);

	EndDrawing();
}

Player& Game::get_opponent(int player_index) {
	Player& opponent = players[1 - player_index];
	return opponent;
}

int Game::find_ship(Player& p, int x, int y) {
	for (int i = 0; i < p.ship_count; i++) {
		Ship& ship = p.ships[i];
		if (ship.x <= x && x < ship.x + ship.width
			&& ship.y <= y && y < ship.y + ship.height) {
			return i;
		}
	}
	return -1;
}

int Game::get_hovered_cell_x() {
	int x = (mouse_get_x_world() - board_xoff) / 16;
	x = clamp(x, 0, WIDTH - 1);
	return x;
}

int Game::get_hovered_cell_y() {
	int y = (mouse_get_y_world() - board_yoff) / 16;
	y = clamp(y, 0, HEIGHT - 1);
	return y;
}

Ship Game::get_hovered_ship(int width, int height) {
	int x = get_hovered_cell_x();
	int y = get_hovered_cell_y();

	x = clamp(x, 0, WIDTH  - width);
	y = clamp(y, 0, HEIGHT - height);

	Ship ship = {};
	ship.x = x;
	ship.y = y;
	ship.width = width;
	ship.height = height;

	return ship;
}

void Game::draw_ship(Ship& ship) {
	int xx = 16 * ship.x;
	int yy = 16 * ship.y;

	DrawRectangle(xx, yy, 16 * ship.width, 16 * ship.height, WHITE);

	for (int y = 0; y < ship.height; y++) {
		for (int x = 0; x < ship.width; x++) {
			if (ship.is_hit[y][x]) {
				DrawCross(xx + 16 * x, yy + 16 * y, 16, 16, RED);
			}
		}
	}
}

void Game::draw_player_ships(Player& p) {
	for (int i = 0; i < p.ship_count; i++) {
		draw_ship(p.ships[i]);
	}
}
