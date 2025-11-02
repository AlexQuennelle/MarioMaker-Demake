#include "game.h"

#include <imgui.h>
#include <memory>
#include <raylib.h>
#include <rlImGui.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

std::unique_ptr<Game> game{nullptr};

void Update();

int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
#if defined(PLATFORM_WEB)
	InitWindow(800, 450, NAME);
	SetTargetFPS(60);
	rlImGuiSetup(true);
	game = std::make_unique<Game>();
	emscripten_set_main_loop(Update, 0, 1);
#else
	InitWindow(1600, 900, NAME);
	SetTargetFPS(60);
	rlImGuiSetup(true);
	game = std::make_unique<Game>();
	while (!WindowShouldClose())
	{
		Update();
	}
#endif

	game.reset();

	rlImGuiShutdown();
	CloseWindow();

	return 0;
}

void Update() { game->Update(); }
