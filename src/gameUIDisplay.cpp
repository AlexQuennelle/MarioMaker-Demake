#include "gameUIDisplay.h"

GameUIDisplay::GameUIDisplay(Font& font) : font(font)
{
	renderTex = LoadRenderTexture(384, 216);
}

GameUIDisplay::~GameUIDisplay() 
{ 
	UnloadRenderTexture(renderTex); 
}

void GameUIDisplay::Draw(const float time)
{
	BeginTextureMode(this->renderTex);
	DrawTextEx(font, "TIME", {0, 0},
			   static_cast<float>(font.baseSize) / 4, 0, YELLOW);
	EndTextureMode();

	// Draw scaled up render texture
	DrawTexturePro(this->renderTex.texture,
				   {0.0f, 0.0f,
					static_cast<float>(this->renderTex.texture.width),
					-static_cast<float>(this->renderTex.texture.height)},
				   {0.0f, 0.0f, -static_cast<float>(GetScreenWidth()),
					static_cast<float>(GetScreenHeight())},
				   {0.0f}, 0.0f, WHITE);
}