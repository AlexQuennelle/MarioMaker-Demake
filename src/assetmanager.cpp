#include "assetmanager.h"

#include <raylib.h>

AssetManager::AssetManager()
{
	playerSprites = LoadTexture(RESOURCES_PATH "sprites/playersprites.png");
	groundTiles = LoadImage(RESOURCES_PATH "sprites/groundSprites.png");
	staticEntities = LoadTexture(RESOURCES_PATH "sprites/staticEntities.png");
}

AssetManager::~AssetManager() {
	UnloadTexture(playerSprites);
	//UnloadImage(this->groundTiles);
	//UnloadTexture(this->staticEntities);
}

PlayerAssets::PlayerAssets(Texture2D& tex) : sprites(tex) {}
