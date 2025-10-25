#include "game.h"
#include "utils.h"

#include <fstream>
#include <imgui.h>
#include <ios>
#include <iostream>
#include <nfd.h>
#include <nfd.hpp>
#include <raylib.h>
#include <rlImGui.h>

Game::Game()
	: imguiIO(ImGui::GetIO()), player(level), inputHandler(player),
	  gravity(1.5f), renderTex(LoadRenderTexture(384, 224))
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
	BeginTextureMode(this->renderTex);
	this->Draw();
	// call update on everything
	// input gets polled first :craigthumb:
	inputHandler.Update();

	player.AddForce({0, gravity * GetFrameTime()});
	player.Update();

	// HACK: Input should be handled more gracefully.
	if (IsKeyPressed(KEY_ENTER))
	{
		this->SaveLevel();
	}

	// draw everything
	Draw();
}

void Game::Draw()
{
	ClearBackground({100, 149, 237, 255});

	this->level.Draw();

	this->player.Draw();

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

void Game::Reset()
{
	level.Reset();
	player.Reset(level.GetPlayerStartPos());
}

void Game::SaveLevel()
{
	NFD::Guard nfdGuard;
	NFD::UniquePath outPath;
	//nfdresult_t result = NFD::OpenDialog(outPath);
	nfdresult_t result =
		NFD::SaveDialog(outPath, nullptr, 0, RESOURCES_PATH, "MyLevel.lvl");
	if (result == NFD_OKAY)
	{
		std::cout << outPath.get() << '\n';
		std::ofstream outFile{
			outPath.get(),
			std::ios::out | std::ios::binary,
		};

		if (outFile.is_open())
		{
			const vector<byte> data{this->level.Serialize()};

			outFile.write(reinterpret_cast<const char*>(data.data()),
						  data.size());

			outFile.close();
		}
		else
		{
			SetTextColor(ERROR);
			std::cout << "ERROR: could not open " << outPath.get() << '\n';
		}
	}
	else if (result == NFD_ERROR)
	{
		std::cout << NFD_GetError() << '\n';
	}
	else if (result == NFD_CANCEL)
	{
		std::cout << "Save cancelled.\n";
	}
}
