#pragma once

#include <raylib.h>
#include <memory>

class AssetManager
{
	public:
	AssetManager();
	~AssetManager();
	Texture2D playerSprites;
	Font smallFont;
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