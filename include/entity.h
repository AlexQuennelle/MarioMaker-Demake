#pragma once

#include "assetmanager.h"
#include "raylib.h"

class Entity
{
	public:
	Entity(const int x, const int y /*, Texture& sprites*/);

	virtual void Update() = 0;
	virtual void Draw() = 0;
	// IDK this might be stupid
	virtual bool Bonk(const bool isPlayer, const bool big) = 0;

	protected:
	bool isActive{true};
	//Texture& sprites;
	Vector2 position;
	Rectangle collider{0.0f, 0.0f, 1.0f, 1.0f};
};

class Brick : public Entity
{
	public:
	Brick(const int x, const int y, const AssetManager& assetManager);

	void Update() override;
	void Draw() override;
	bool Bonk(const bool isPlayer, const bool big) override;

	private:
};
