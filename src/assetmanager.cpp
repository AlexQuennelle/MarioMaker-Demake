#include "assetmanager.h"

AssetManager::AssetManager()
{
	playerSprites = LoadTexture(RESOURCES_PATH "sprites/playersprites.png");
}

AssetManager::~AssetManager() { UnloadTexture(playerSprites); }

PlayerAssets::PlayerAssets(Texture2D& tex) : sprites(tex) {}
