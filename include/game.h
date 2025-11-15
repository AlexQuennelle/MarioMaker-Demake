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
	~Game();

	/**
	 * The game's main update loop. Should be called  from a loop in main().
	 */
	void Update();

	private:
	void Draw();

	void SwitchMode(SwitchRequest newMode);

	void Reset();
	void SaveLevel();
#if !defined(PLATFORM_WEB)
	void SaveLevelAs();
	void LoadLevel();
#endif

	RenderTexture2D renderTex;
	std::unique_ptr<GamemodeInstance> gamemode;
	std::unique_ptr<Level> level{nullptr};
	ImGuiIO& imguiIO;
	asset_ptr assetManager;
};
