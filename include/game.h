#pragma once

#include "assetmanager.h"
#include "gamemode.h"
#include "level.h"

#include <imgui.h>
#include <memory>
#include <raylib.h>

/**
 * @brief Class representing an instance of the program and encapsulating
 *        global state.
 */
class Game // NOLINT
{
	public:
	Game();
	~Game();

	/**
	 * @brief The game's main update loop. Should be called  from a loop in
	 *        main().
	 */
	void Update();

	private:
	void Draw();

	/**
	 * @brief Switches what game mode object is currently being used. Game mode
	 *        objects are derived from the abstract @link GamemodeInstance
	 *        @endlink class
	 */
	void SwitchMode(SwitchRequest newMode);

	RenderTexture2D renderTex;
	std::unique_ptr<GamemodeInstance> gamemode;
	std::unique_ptr<Level> level{nullptr};
	ImGuiIO& imguiIO;
	asset_ptr assetManager;
};
