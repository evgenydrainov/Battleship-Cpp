#include "game.h"

#include "util.h"
#include "mathh.h"
#include <rlgl.h>
#include <raymath.h>

Game* game;

void Game::init() {
	InitWindow(3 * GAME_W, 3 * GAME_H, "Battleship-C++");
	SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

	rlDisableBackfaceCulling();

	game_texture = LoadRenderTextureNoAlphaNoDepth(GAME_W, GAME_H);
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

}

void Game::draw(float delta) {
	BeginDrawing();

	BeginTextureMode(game_texture);
	ClearBackground({100, 149, 237, 255});

	{
		Matrix proj = MatrixOrtho(0, GAME_W, 0, GAME_H, -1, 1);
		rlSetMatrixProjection(proj);
	}

	for (int y = 0; y < HEIGHT + 1; y++) {
		for (int x = 0; x < WIDTH + 1; x++) {
			DrawRectangleLines(16 * x, 16 * y, 17, 17, WHITE);
		}
	}

	for (int x = 0; x < WIDTH; x++) {
		char buf[] = {'A' + x, 0};
		DrawText(buf, 16 * (x + 1) + 6, 4, 10, WHITE);
	}

	for (int y = 0; y < HEIGHT; y++) {
		DrawText(TextFormat("%d", y + 1), 4, 16 * (y + 1) + 4, 10, WHITE);
	}

	{
		int x = mouse_get_x_world() / 16 - 1;
		int y = mouse_get_y_world() / 16 - 1;

		Ship ship = {};
		ship.height = 4;

		x = clamp(x, 0, WIDTH - ship.width);
		y = clamp(y, 0, HEIGHT - ship.height);

		ship.x = x;
		ship.y = y;

		draw_ship(ship);
	}

	EndTextureMode();

	ClearBackground(BLACK);

	{
		Matrix proj = MatrixOrtho(0, GAME_W, GAME_H, 0, -1, 1);
		rlSetMatrixProjection(proj);
	}

	DrawTexture(game_texture.texture, 0, 0, WHITE);

	EndDrawing();
}

void Game::draw_ship(Ship& ship) {
	int x = (ship.x + 1) * 16;
	int y = (ship.y + 1) * 16;
	DrawRectangle(x, y, 16 * ship.width, 16 * ship.height, WHITE);
}
