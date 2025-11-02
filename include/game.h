#pragma once

#include "assetmanager.h"
#include "gamemode.h"
#include "level.h"

#include <imgui.h>
#include <memory>
#include <raylib.h>

/**
 * Class representing an instance of the program and encapsulating global state.
 */
class Game
{
	public:
	Game();

	/**
	 * The game's main update loop. Should be called  from a loop in main().
	 */
	void Update();

	private:
	void Draw();

	void Reset();
	void SaveLevel();
#if !defined(PLATFORM_WEB)
	void SaveLevelAs();
	void LoadLevel();
#endif

	std::unique_ptr<GamemodeInstance> gamemode;
	Level level;

	ImGuiIO& imguiIO;
	RenderTexture2D renderTex;
	asset_ptr assetManager;
};
