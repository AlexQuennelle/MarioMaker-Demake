#include "program.h"

#include <imgui.h>
#include <memory>
#include <raylib.h>
#include <rlImGui.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

std::unique_ptr<Program> program{nullptr};

void Update();

int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
#if defined(PLATFORM_WEB)
	InitWindow(400, 400, NAME);
	rlImGuiSetup(true);
	emscripten_set_main_loop(Update, 0, 1);
#else
	InitWindow(800, 800, NAME);
	SetTargetFPS(60);
	rlImGuiSetup(true);
	program = std::make_unique<Program>();
	while (!WindowShouldClose())
	{
		Update();
	}
#endif

	rlImGuiShutdown();
	CloseWindow();

	return 0;
}

void Update() { program->Update(); }
