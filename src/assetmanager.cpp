#include <raylib.h>

#include "assetmanager.h"

AssetManager::AssetManager()
{
	playerSprites = LoadTexture(RESOURCES_PATH "sprites/playersprites.png");
	smallFont =
		LoadFontEx(RESOURCES_PATH "fonts/nokiafc22.ttf", 40, nullptr, 250);
	SetTextureFilter(smallFont.texture, TEXTURE_FILTER_POINT);
	groundTiles = LoadImage(RESOURCES_PATH "sprites/groundSprites.png");
	staticEntities = LoadTexture(RESOURCES_PATH "sprites/staticEntities.png");
	powerups = LoadTexture(RESOURCES_PATH "sprites/powerups.png");
}

AssetManager::~AssetManager()
{
	UnloadTexture(this->playerSprites);
	UnloadImage(this->groundTiles);
	UnloadTexture(this->staticEntities);
	UnloadTexture(this->powerups);
}

PlayerAssets::PlayerAssets(Texture2D& tex) : sprites(tex) {}
