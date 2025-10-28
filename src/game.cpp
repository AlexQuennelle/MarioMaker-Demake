#include "game.h"
#include "gamemode.h"
#include "level.h"
#include "utils.h"

#include <fstream>
#include <imgui.h>
#include <ios>
#include <iostream>
#include <memory>
#include <nfd.h>
#include <nfd.hpp>
#include <raylib.h>
#include <rlImGui.h>

Game::Game()
	: imguiIO(ImGui::GetIO()), renderTex(LoadRenderTexture(384, 224)),
	  // HACK: Temporarily hardcode in path to level file
	  level(RESOURCES_PATH "MyLevel.lvl")
{
	SetTextColor(INFO);
	std::cout << "Initializing...\n";

	//this->LoadLevel();

	//this->gamemode = std::make_unique<GameplayMode>(this->level);
	this->gamemode = std::make_unique<EditMode>(this->level);

	imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	std::cout << "Done!\n";
	ClearStyles();
}

void Game::Update()
{
	BeginTextureMode(this->renderTex);
	ClearBackground({100, 149, 237, 255});
	BeginMode2D(this->gamemode->camera);

	this->gamemode->Update();

	// draw everything
	Draw();
}

void Game::Draw()
{
	this->gamemode->Draw();

	EndMode2D();
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

	this->gamemode->DrawUI();

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

void Game::SaveLevel()
{
	std::ofstream outFile;
	if (this->level.HasFilepath())
	{
		outFile = std::ofstream(this->level.GetFilepath(),
								std::ios::out | std::ios::binary);
	}
	else
	{
		NFD::Guard nfdGuard;
		NFD::UniquePath outPath;
		nfdfilteritem_t filter{"Level Files", "lvl"};
		nfdresult_t result{NFD::SaveDialog(outPath, &filter, 1, RESOURCES_PATH,
										   "MyLevel.lvl")};
		if (result == NFD_OKAY)
		{
			this->level.SetFilepath(outPath.get());
			outFile =
				std::ofstream(outPath.get(), std::ios::out | std::ios::binary);
		}
		else if (result == NFD_ERROR)
		{
			std::cout << NFD_GetError() << '\n';
			return;
		}
		else if (result == NFD_CANCEL)
		{
			std::cout << "Save cancelled.\n";
			return;
		}
	}

	if (outFile.is_open())
	{
		const vector<byte> data{this->level.Serialize()};

		outFile.write(reinterpret_cast<const char*>(data.data()), data.size());

		outFile.close();
	}
	else
	{
		SetTextColor(ERROR);
		std::cerr << "ERROR: could not open file." << '\n';
		ClearStyles();
	}
}
void Game::SaveLevelAs()
{
	this->level.SetFilepath("");
	this->SaveLevel();
}
void Game::LoadLevel()
{
	NFD::Guard nfdGuard;
	NFD::UniquePath outPath;
	nfdfilteritem_t filter{"Level Files", "lvl"};
	nfdresult_t result{NFD::OpenDialog(outPath, &filter, 1, RESOURCES_PATH)};
	if (result == NFD_OKAY)
	{
		std::cout << outPath.get() << '\n';
		this->level = Level(outPath.get());
	}
	else if (result == NFD_ERROR)
	{
		std::cerr << NFD_GetError() << '\n';
		return;
	}
	else if (result == NFD_CANCEL)
	{
		std::cout << "Load cancelled.\n";
		return;
	}
}
