#pragma once
#include "player.h"

class PlayerInputHandler
{
	public:
	PlayerInputHandler(Player& player);
	void Update();

	private:
	Player& player;
};