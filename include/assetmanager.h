#pragma once

#include <memory>
#include <raylib.h>

class AssetManager
{
	public:
	AssetManager();
	~AssetManager();

	Texture2D playerSprites;
	Texture2D staticEntities;
	Texture2D powerups;
	Texture2D enemies;
	Font smallFont;
	Image groundTiles;
};

struct PlayerAssets
{
	public:
	PlayerAssets(Texture2D&, Texture2D&);

	Texture2D& sprites;
	Texture2D& staticEntities;
	float fireOffset{288};
	float luigiOffset{128};
};

using asset_ptr = std::unique_ptr<AssetManager>;
