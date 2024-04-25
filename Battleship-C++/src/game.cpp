#include "game.h"

#include "util.h"
#include "mathh.h"
#include <rlgl.h>
#include <raymath.h>
#include <string.h>

// #define COLOR_BG {100, 149, 237, 255}
#define COLOR_BG WHITE
#define COLOR_TEXT BLACK
#define COLOR_HIT RED

#define MAKE_YOUR_TURN_MSG "Make your turn. Hold space to view your board."

Game* game;

static int ship_lengths_to_place[PLAYER_MAX_SHIPS] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};

static void log_callback(int logLevel, const char *text, va_list args) {}

void Game::init() {
	out.open("out.txt");
	coutbuf = std::cout.rdbuf();
	std::cout.rdbuf(out.rdbuf());

	SetTraceLogCallback(log_callback);

	InitWindow(3 * GAME_W, 3 * GAME_H, "Battleship-C++");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

	rlDisableBackfaceCulling();

	game_texture = LoadRenderTextureNoAlphaNoDepth(GAME_W, GAME_H);

	state = GAME_STATE_PLACING_SHIPS;
	player_index = 0;
	placing_ship_w = 1;
	placing_ship_h = ship_lengths_to_place[placing_ship_index];
	message = "Place your ships.";
}

void Game::destroy() {
	UnloadRenderTexture(game_texture);

	CloseWindow();

	std::cout.rdbuf(coutbuf);
	out.close();
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
				memset(p.board, 0, sizeof p.board);
			}

			if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) || IsKeyPressed(KEY_SPACE)) {
				int temp = placing_ship_w;
				placing_ship_w = placing_ship_h;
				placing_ship_h = temp;
			}

			placing_ship_x = clamp(get_hovered_cell_x(), 0, WIDTH -  placing_ship_w);
			placing_ship_y = clamp(get_hovered_cell_y(), 0, HEIGHT - placing_ship_h);

			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				Player& p = players[player_index];

				if (can_place_ship(p, placing_ship_x, placing_ship_y, placing_ship_w, placing_ship_h)) {
					for (int y = 0; y < placing_ship_h; y++) {
						for (int x = 0; x < placing_ship_w; x++) {
							p.board[placing_ship_y + y][placing_ship_x + x] |= CELL_IS_SHIP;
						}
					}

					std::cout
						<< "place "
						<< player_index << " "
						<< (char)('A' + placing_ship_x) << placing_ship_y + 1 << " "
						<< placing_ship_w << " "
						<< placing_ship_h << "\n";

					placing_ship_index++;

					if (placing_ship_index >= PLAYER_MAX_SHIPS) {
						player_index++;
						placing_ship_index = 0;

						if (player_index >= MAX_PLAYERS) {
							state = GAME_STATE_PLAYING;
							player_index = 0;
							message = MAKE_YOUR_TURN_MSG;
						}
					}

					placing_ship_w = 1;
					placing_ship_h = ship_lengths_to_place[placing_ship_index];

					// for draw
					placing_ship_x = clamp(get_hovered_cell_x(), 0, WIDTH -  placing_ship_w);
					placing_ship_y = clamp(get_hovered_cell_y(), 0, HEIGHT - placing_ship_h);
				}
			}

			break;
		}

		case GAME_STATE_PLAYING: {

			if (wait_timer > 0) {
				wait_timer -= delta;
				if (wait_timer <= 0) {
					player_index++;
					player_index %= MAX_PLAYERS;
					message = MAKE_YOUR_TURN_MSG;
				}
				break;
			}

			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !IsKeyDown(KEY_SPACE)) {
				int x = get_hovered_cell_x();
				int y = get_hovered_cell_y();

				Player& p = players[player_index];
				Player& opponent = get_opponent(player_index);

				if (!(opponent.board[y][x] & CELL_IS_HIT)) {
					opponent.board[y][x] |= CELL_IS_HIT;

					std::cout
						<< "hit "
						<< player_index << " "
						<< (char)('A' + x) << y + 1 << "\n";

					if (opponent.board[y][x] & CELL_IS_SHIP) {
						message = "Hit! Make another turn.";
					} else {
						message = "Miss!";
						wait_timer = 60;
					}

					int ship_cells = 0;
					for (int y = 0; y < HEIGHT; y++) {
						for (int x = 0; x < WIDTH; x++) {
							if ((opponent.board[y][x] & CELL_IS_SHIP) && !(opponent.board[y][x] & CELL_IS_HIT)) {
								ship_cells++;
							}
						}
					}

					if (ship_cells == 0) {
						state = GAME_STATE_WON;
						message = "You won!";
						wait_timer = 1;
					}
				}
			}
			break;
		}

		case GAME_STATE_WON: {
			break;
		}
	}
}

void Game::draw(float delta) {
	BeginDrawing();

	BeginTextureMode(game_texture);
	ClearBackground(COLOR_BG);

	{
		rlDrawRenderBatchActive();

		Matrix proj = MatrixOrtho(0, GAME_W, 0, GAME_H, -1, 1);
		rlSetMatrixProjection(proj);

		Matrix modelview = MatrixTranslate(board_xoff, board_yoff, 0);
		rlSetMatrixModelview(modelview);
	}

	for (int y = 0; y < HEIGHT + 1; y++) {
		for (int x = 0; x < WIDTH + 1; x++) {
			DrawRectangleLines(16 * (x - 1), 16 * (y - 1), 17, 17, COLOR_TEXT);
		}
	}

	for (int x = 0; x < WIDTH; x++) {
		char buf[] = {'A' + x, 0};
		DrawText(buf, 16 * x + 6, 4 - 16, 10, COLOR_TEXT);
	}

	for (int y = 0; y < HEIGHT; y++) {
		DrawText(TextFormat("%d", y + 1), 4 - 16, 16 * y + 4, 10, COLOR_TEXT);
	}

	switch (state) {
		case GAME_STATE_PLACING_SHIPS: {
			Player& p = players[player_index];
			draw_player_board(p);

			DrawRectangle(16 * placing_ship_x, 16 * placing_ship_y, 16 * placing_ship_w, 16 * placing_ship_h, COLOR_TEXT);
			break;
		}

		case GAME_STATE_PLAYING:
		case GAME_STATE_WON: {
			if (IsKeyDown(KEY_SPACE)) {
				Player& p = players[player_index];
				draw_player_board(p);
			} else {
				Player& opponent = get_opponent(player_index);
				draw_player_board(opponent, false);

				int x = get_hovered_cell_x();
				int y = get_hovered_cell_y();

				if (wait_timer <= 0) {
					DrawCross(16 * x, 16 * y, 16, 16, COLOR_HIT);
				}
			}
			break;
		}
	}

	rlDrawRenderBatchActive();
	rlSetMatrixModelview(MatrixIdentity());

	{
		int y = board_yoff + 16 * HEIGHT + 10;

		DrawText(TextFormat("Player %d", player_index + 1), 16, y, 10, COLOR_TEXT);
		y += 10;

		DrawText(message, 16, y, 10, COLOR_TEXT);
		y += 10;
	}

	EndTextureMode();

	ClearBackground(BLACK);

	{
		rlDrawRenderBatchActive();

		Matrix proj = MatrixOrtho(0, GAME_W, GAME_H, 0, -1, 1);
		rlSetMatrixProjection(proj);
	}

	DrawTexture(game_texture.texture, 0, 0, WHITE);

	EndDrawing();
}

Player& Game::get_opponent(int player_index) {
	Player& opponent = players[1 - player_index];
	return opponent;
}

bool Game::can_place_ship(Player& p, int ship_x, int ship_y, int ship_w, int ship_h) {
	for (int yoff = -1; yoff < ship_h + 1; yoff++) {
		for (int xoff = -1; xoff < ship_w + 1; xoff++) {
			int x = clamp(ship_x + xoff, 0, WIDTH  - 1);
			int y = clamp(ship_y + yoff, 0, HEIGHT - 1);

			if (p.board[y][x] & CELL_IS_SHIP) {
				return false;
			}
		}
	}
	return true;
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

void Game::draw_player_board(Player& p, bool show_ships) {
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (show_ships && p.board[y][x] & CELL_IS_SHIP) {
				DrawRectangle(16 * x, 16 * y, 16, 16, COLOR_TEXT);
			}

			if (p.board[y][x] & CELL_IS_HIT) {
				if (p.board[y][x] & CELL_IS_SHIP) {
					DrawCross(16 * x, 16 * y, 16, 16, COLOR_HIT);
				} else {
					DrawCircle(16 * x + 8, 16 * y + 8, 3, COLOR_HIT);
				}
			}
		}
	}
}
