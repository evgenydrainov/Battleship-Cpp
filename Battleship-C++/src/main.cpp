#include "game.h"

#ifdef NDEBUG
int __stdcall WinMain(void* hInst, void* hInstPrev, char* cmdline, int cmdshow) {
#else
int main(int argc, char* argv[]) {
#endif
	Game game_instance = {};
	game = &game_instance;

	game->init();
	game->run();
	game->destroy();

	return 0;
}
