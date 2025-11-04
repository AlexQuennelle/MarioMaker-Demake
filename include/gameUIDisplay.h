#pragma once

#include <raylib.h>

class GameUIDisplay
{
	public:
	GameUIDisplay(Font& font);
	~GameUIDisplay();
	void Draw(const float);

	private:
	Font& font;
	RenderTexture2D renderTex;
};