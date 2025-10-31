#include "assetmanager.h"
#include "raylib.h"

AssetManager::AssetManager()
{
	playerSprites = LoadTexture(RESOURCES_PATH "sprites/playersprites.png");
	smallFont = LoadFontEx(RESOURCES_PATH "fonts/nokiafc22.ttf", 8, 0, 250);
	groundTiles = LoadImage(RESOURCES_PATH "sprites/groundSprites.png");
}

AssetManager::~AssetManager() 
{ 
	UnloadTexture(playerSprites);
	UnloadFont(smallFont);
}
	

PlayerAssets::PlayerAssets(Texture2D& tex) : sprites(tex) {}
