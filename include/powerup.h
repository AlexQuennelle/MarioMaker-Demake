#pragma once

#include "entity.h"

class Mushroom : public Entity
{
	public:
	Mushroom(const int x, const int y, AssetManager& assetManager,
			 const float gravity);

	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	void Draw() override;
	void EditDraw() override;
	auto OnPlayerCollision(Player& player) -> EntityReq override;
	auto OnEntityCollision(Entity& entity) -> EntityReq override;

	private:
	Texture2D& sprite;
	Vector2 velocity{0, 0};
	float speed{0.05f};
	float gravity;
};

class FireFlower : public Entity
{
	public:
	FireFlower(const int x, const int y, AssetManager& assetManager,
			   const float gravity);

	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	void Draw() override;
	void EditDraw() override;
	auto OnPlayerCollision(Player& player) -> EntityReq override;
	auto OnEntityCollision(Entity& entity) -> EntityReq override;

	private:
	Texture2D& sprite;
	Vector2 velocity{0, 0};
	float gravity;
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.12f};
	int curFrame{0};
};
