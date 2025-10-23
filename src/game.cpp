#include "game.h"
#include "utils.h"

#include <imgui.h>
#include <iostream>
#include <raylib.h>
#include <rlImGui.h>

Game::Game() : imguiIO(ImGui::GetIO()), renderTex(LoadRenderTexture(384, 224))
{
	SetTextColor(INFO);
	std::cout << "Initializing...\n";

	imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	std::cout << "Done!\n";
	ClearStyles();
}

void Game::Update()
{
	BeginTextureMode(this->renderTex);
	this->Draw();
}

void Game::Draw()
{
	ClearBackground({100, 149, 237, 255});

	this->level.Draw();

	EndTextureMode();

	BeginDrawing();
	rlImGuiBegin();

	// Draw scaled up render texture
	DrawTexturePro(this->renderTex.texture,
				   {0.0f, 0.0f,
					static_cast<float>(this->renderTex.texture.width),
					-static_cast<float>(this->renderTex.texture.height)},
				   {0.0f, 0.0f, -static_cast<float>(GetScreenWidth()),
					static_cast<float>(GetScreenHeight())},
				   {0.0f}, 0.0f, WHITE);

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

void Game::Reset() { level.Reset(); }
