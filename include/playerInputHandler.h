#pragma once

#include "player.h"

class PlayerInputHandler
{
	public:
	PlayerInputHandler(Player& player);
	void Update();

	auto IsPaused() const -> bool { return this->paused; }

	private:
	Player& player;
	bool paused{false};
};
