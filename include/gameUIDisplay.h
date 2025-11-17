#pragma once

#include <raylib.h>

class GameUIDisplay
{
	public:
	GameUIDisplay(Font& font);
	~GameUIDisplay();
	void Draw(const float, const int);

	private:
	RenderTexture2D renderTex;
	Font& font;
};
