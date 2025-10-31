#include "assetmanager.h";
#include "utils.h";

AssetManager::AssetManager()
{
	playerSprites = LoadTexture(RESOURCES_PATH "sprites/playersprites.png");
	smallFont = LoadFontEx(RESOURCES_PATH "fonts/nokiafc22.ttf", 8, 0, 250);
}

AssetManager::~AssetManager() 
{ 
	UnloadTexture(playerSprites);
	UnloadFont(smallFont);
}

PlayerAssets::PlayerAssets(Texture2D &tex) : sprites(tex) {

}