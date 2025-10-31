#pragma once

#include <raylib.h>

class GameUIDisplay
{
	public:
	GameUIDisplay(Font& font);
	void Draw(const float);

	private:
	Font& font;
};