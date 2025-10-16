#include "playerInputHandler.h"
#include "utils.h"

PlayerInputHandler::PlayerInputHandler(Player& _player) : player(_player)
{
	
}

void PlayerInputHandler::Update() 
{ 
	bool running{IsKeyDown(KEY_LEFT_SHIFT)};

	if (IsKeyDown(KEY_RIGHT || KEY_D))
	{
		player.HandleMovement(running, 1.0f);
	}
	if (IsKeyDown(KEY_LEFT || KEY_A))
	{
		player.HandleMovement(running, -1.0f);
	}
}