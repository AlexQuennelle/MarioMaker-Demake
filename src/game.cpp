#include "game.h"
#include "constants.h"
#include "gamemode.h"
#include "utils.h"
#if defined(PLATFORM_WEB)
#include "wasmUtils.h"
#endif

#include <cassert>
#include <imgui.h>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <rlImGui.h>
#if !defined(PLATFORM_WEB)
#include <nfd.h>
#include <nfd.hpp>
#endif

Game::Game() :
	renderTex(LoadRenderTexture(384, 216)),
	imguiIO(ImGui::GetIO()),
	assetManager(std::make_unique<AssetManager>())
{
	SetTextColor(INFO);
	std::cout << "Initializing...\n";

	this->gamemode
		= std::make_unique<MainMenu>(*this->assetManager, this->level);

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
				   {0.0f, 0.0f}, 0.0f, WHITE);

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
		this->gamemode
			= std::make_unique<MainMenu>(*this->assetManager, this->level);
		break;
	}
}
