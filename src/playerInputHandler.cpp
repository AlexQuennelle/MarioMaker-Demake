#include "playerInputHandler.h"

PlayerInputHandler::PlayerInputHandler(Player& _player) : player(_player) {}

void PlayerInputHandler::Update()
{
	bool running{IsKeyDown(KEY_LEFT_SHIFT)};

	Vector2 input{0, 0};

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
}
