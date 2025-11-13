#include "constants.h"
#include "game.h"

#include <imgui.h>
#include <memory>
#include <raylib.h>
#include <rlImGui.h>
#if defined(PLATFORM_WEB)
#include "wasmUtils.h"
#include <emscripten/emscripten.h>
#endif

std::unique_ptr<Game> game{nullptr};

void Update();

int main()
{
#if defined(PLATFORM_WEB)
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT + EDIT_PANEL_HEIGHT, NAME);
	SetTargetFPS(60);
	rlImGuiSetup(true);
	game = std::make_unique<Game>();
	requestSize(SCREEN_WIDTH, SCREEN_HEIGHT + EDIT_PANEL_HEIGHT);
	emscripten_set_main_loop(Update, 0, 1);
#else
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, NAME);
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
