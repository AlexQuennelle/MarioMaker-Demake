#include "assetmanager.h";
#include "utils.h";

AssetManager::AssetManager()
{
	playerSprites = LoadTexture(RESOURCES_PATH "sprites/playersprites.png");
}

AssetManager::~AssetManager() 
{ UnloadTexture(playerSprites); }