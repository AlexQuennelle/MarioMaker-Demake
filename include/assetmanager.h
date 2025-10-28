#pragma once

#include "utils.h"

class AssetManager
{
	public:
	AssetManager();
	~AssetManager();
	Texture2D playerSprites;
};

struct PlayerAssets
{
	public:
	PlayerAssets(Texture2D&);
	Texture2D& sprites;
	float fireOffset{288};
	float luigiOffset{128};
};

enum class PlayerAnimState
{
	idle,
	lookUp,
	crouch,
	walk,
	run,
	skid,
	jump,
	fall,
	runJump,
	victory,
	death,
};