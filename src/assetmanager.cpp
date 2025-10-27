#include "assetmanager.h";
#include "utils.h";

AssetManager::AssetManager()
{
	playerSprites = LoadImage(RESOURCES_PATH "sprites/playersprites.png");
}

AssetManager::~AssetManager() 
{ UnloadImage(playerSprites); }