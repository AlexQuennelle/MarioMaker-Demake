#include "gameUIDisplay.h"
#include "constants.h"

#include <cmath>
#include <string>

GameUIDisplay::GameUIDisplay(Font& font) : font(font)
{
	renderTex = LoadRenderTexture(384, 216);
}

GameUIDisplay::~GameUIDisplay() { UnloadRenderTexture(renderTex); }

void GameUIDisplay::Draw(const float time, const int coins)
{
	BeginTextureMode(this->renderTex);
	ClearBackground(BLANK);
	DrawTextEx(font, "TIME", {348, 8}, static_cast<float>(font.baseSize) / 4, 0,
			   WHITE);
	DrawTextEx(font, std::to_string(static_cast<int>(std::ceil(time))).c_str(),
			   {348, 16}, static_cast<float>(font.baseSize) / 4, 0, WHITE);
	DrawTextEx(font, TextFormat("COINS: %02i", coins), {16, 8},
			   static_cast<float>(font.baseSize) / 4, 0, WHITE);
	EndTextureMode();

	// Draw scaled up render texture
	DrawTexturePro(this->renderTex.texture,
				   {0.0f, 0.0f,
					static_cast<float>(this->renderTex.texture.width),
					-static_cast<float>(this->renderTex.texture.height)},
				   {0.0f, 0.0f, -static_cast<float>(SCREEN_WIDTH),
					static_cast<float>(SCREEN_HEIGHT)},
				   {0.0f}, 0.0f, WHITE);
}
