#include "assetmanager.h"
#include "raylib.h"

AssetManager::AssetManager()
{
	playerSprites = LoadTexture(RESOURCES_PATH "sprites/playersprites.png");
	groundTiles = LoadImage(RESOURCES_PATH "sprites/groundSprites.png");
}

AssetManager::~AssetManager() { UnloadTexture(playerSprites); }

PlayerAssets::PlayerAssets(Texture2D& tex) : sprites(tex) {}
