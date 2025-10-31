#pragma once

#include <cstdint>
#include <memory>
#include <raylib.h>

class AssetManager
{
	public:
	AssetManager();
	~AssetManager();
	Texture2D playerSprites;
	Font smallFont;
	Image groundTiles;
};

struct PlayerAssets
{
	public:
	PlayerAssets(Texture2D&);
	Texture2D& sprites;
	float fireOffset{288};
	float luigiOffset{128};
};

using asset_ptr = std::unique_ptr<AssetManager>;
