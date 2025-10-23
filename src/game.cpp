#include "game.h"
#include "utils.h"

#include <imgui.h>
#include <iostream>
#include <raylib.h>
#include <rlImGui.h>

Game::Game()
	: imguiIO(ImGui::GetIO()), level(), player(level), inputHandler(player),
	  gravity(0.9f)
{
	SetTextColor(INFO);
	std::cout << "Initializing...\n";

	imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	std::cout << "Done!\n";
	ClearStyles();

	player.Reset(level.GetPlayerStartPos());
}

void Game::Update()
{
	BeginDrawing();
	rlImGuiBegin();

	// call update on everything
	// input gets polled first :craigthumb:
	inputHandler.Update();

	player.AddForce({0, gravity * GetFrameTime()});
	player.Update();

	// draw everything
	Draw();

	rlImGuiEnd();
	EndDrawing();
}

void Game::Draw() {
	// Draw loop start
	ClearBackground({100, 149, 237, 255});

	level.Draw();

	player.Draw();
}

void Game::Reset()
{
	level.Reset();
	player.Reset(level.GetPlayerStartPos());
}
