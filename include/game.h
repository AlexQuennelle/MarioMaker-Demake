#pragma once
#include "player.h"
#include "playerInputHandler.h"

#include "level.h"

#include <imgui.h>

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

	ImGuiIO& imguiIO;
	Player player;
	PlayerInputHandler inputHandler;

	Level level;

	float gravity;
};
