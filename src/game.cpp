#include "game.h"
#include "utils.h"

#include <imgui.h>
#include <iostream>
#include <raylib.h>
#include <rlImGui.h>

Game::Game() : imguiIO(ImGui::GetIO())
{
	SetTextColor(INFO);
	std::cout << "Initializing...\n";

	imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	std::cout << "Done!\n";
	ClearStyles();
}

void Game::Update()
{
	BeginDrawing();
	rlImGuiBegin();

	// The internal draw loops for Raylib and ImGui starts before the user
	// defined draw loop to allow for debug visualizations.

	// Draw loop start
	ClearBackground({100, 149, 237, 255});

	this->level.Draw();

	// ImGui demo
	bool open = true;
	ImGuiWindowFlags flags{ImGuiWindowFlags_NoSavedSettings |
						   ImGuiWindowFlags_AlwaysAutoResize};
	if (ImGui::Begin("ImGui Window", &open, flags))
	{
		ImGui::Text("Text.");
	}
	ImGui::End();

	rlImGuiEnd();
	EndDrawing();
}
