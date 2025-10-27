#pragma once

#include "utils.h"

class AssetManager
{
	public:
	AssetManager();
	~AssetManager();
	Image playerSprites;
};

struct PlayerAssets
{
	public:
	PlayerAssets(Image&);
	Image& sprites;
};