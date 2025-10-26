#pragma once

#include "level.h"

class GamemodeInstance
{
	public:
	GamemodeInstance(Level& lvl) : level(lvl) {};

	protected:
	Level& level;
};
