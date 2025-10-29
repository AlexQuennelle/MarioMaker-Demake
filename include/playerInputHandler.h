#pragma once
#include "player.h"

class PlayerInputHandler
{
	public:
	PlayerInputHandler(Player& player);
	void Update();

	bool IsPaused() const { return this->paused; }

	private:
	Player& player;
	bool paused{false};
};
