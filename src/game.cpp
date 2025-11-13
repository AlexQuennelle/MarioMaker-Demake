#include "game.h"
#include "constants.h"
#include "gamemode.h"
#include "level.h"
#include "utils.h"
#if defined(PLATFORM_WEB)
#include "constants.h"
#include "wasmUtils.h"
#endif

#include <cassert>
#include <fstream>
#include <imgui.h>
#include <ios>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <rlImGui.h>
#if !defined(PLATFORM_WEB)
#include <nfd.h>
#include <nfd.hpp>
#endif

Game::Game()
	: imguiIO(ImGui::GetIO()), renderTex(LoadRenderTexture(384, 216)),
	  assetManager(std::make_unique<AssetManager>())
{
	SetTextColor(INFO);
	std::cout << "Initializing...\n";

	this->gamemode =
		std::make_unique<MainMenu>(*this->assetManager, this->level);

	imguiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	std::cout << "Done!\n";
	ClearStyles();
}

Game::~Game() { UnloadRenderTexture(renderTex); }

void Game::Update()
{
	BeginTextureMode(this->renderTex);
	ClearBackground({100, 149, 237, 255});
	BeginMode2D(this->gamemode->camera);

	this->gamemode->Update();

	SwitchRequest newMode{this->gamemode->GetNextMode()};
	if (newMode != SwitchRequest::None)
	{
		SwitchMode(newMode);
	}

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
				   {0.0f, 0.0f, -static_cast<float>(SCREEN_WIDTH),
					static_cast<float>(SCREEN_HEIGHT)},
				   {0.0f}, 0.0f, WHITE);

	this->gamemode->DrawUI();

	rlImGuiEnd();
	EndDrawing();
}

void Game::SwitchMode(SwitchRequest newMode)
{
	switch (newMode)
	{
	case SwitchRequest::GameplayMode:
#if defined(PLATFORM_WEB)
		requestSize(SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
		this->gamemode = std::make_unique<GameplayMode>(this->level.get(),
														*this->assetManager);
		break;

	case SwitchRequest::EditMode:
#if defined(PLATFORM_WEB)
		requestSize(SCREEN_WIDTH, SCREEN_HEIGHT + EDIT_PANEL_HEIGHT);
#endif
		this->gamemode = std::make_unique<EditMode>(
			this->level.get(), *this->assetManager, this->imguiIO);
		break;

	case SwitchRequest::MainMenu:
	default:
#if defined(PLATFORM_WEB)
		requestSize(SCREEN_WIDTH, SCREEN_HEIGHT);
#endif
		this->level.reset(nullptr);
		this->gamemode =
			std::make_unique<MainMenu>(*this->assetManager, this->level);
		break;
	}
}

void Game::SaveLevel()
{
	std::ofstream outFile;
	if (this->level->HasFilepath())
	{
		outFile = std::ofstream(this->level->GetFilepath(),
								std::ios::out | std::ios::binary);
	}
	else
	{
#if !defined(PLATFORM_WEB)
		NFD::Guard nfdGuard;
		NFD::UniquePath outPath;
		nfdfilteritem_t filter{"Level Files", "lvl"};
		nfdresult_t result{NFD::SaveDialog(outPath, &filter, 1, RESOURCES_PATH,
										   "MyLevel.lvl")};
		if (result == NFD_OKAY)
		{
			this->level->SetFilepath(outPath.get());
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
#endif
	}

	if (outFile.is_open())
	{
		const vector<byte> data{this->level->Serialize()};

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
#if !defined(PLATFORM_WEB)
void Game::SaveLevelAs()
{
	this->level->SetFilepath("");
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
		this->level =
			std::make_unique<Level>(Level{outPath.get(), *this->assetManager});
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
#endif
