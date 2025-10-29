#include "playerInputHandler.h"
#include "utils.h"

PlayerInputHandler::PlayerInputHandler(Player& _player) : player(_player)
{
	
}

void PlayerInputHandler::Update() 
{ 
	// pausing
	if (IsKeyPressed(KEY_P))
	{
		paused = !paused;
	}

	// no need to process player input if paused
	if (paused)
		return;

	bool running{IsKeyDown(KEY_LEFT_SHIFT)};

	Vector2 input{0, 0};

	// movement !
	if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
	{
		input.x += 1.0f;
	}
	if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
	{
		input.x -= 1.0f;
	}
	if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
	{
		input.y += 1.0f;
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
	{
		input.y -= 1.0f;
	}

	player.HandleMovement(running, input);

	bool jump{IsKeyDown(KEY_SPACE)};

	player.HandleJump(jump);

	// HACK: remove when death is possible via hazards
	if (IsKeyDown(KEY_X))
	{
		player.TemporaryDeathTest();
	}
}