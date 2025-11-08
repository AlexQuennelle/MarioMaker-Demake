#pragma once

#include "entity.h"

class WalkerEnemy : public Entity
{
	public:
	WalkerEnemy(const int x, const int y, AssetManager& assetManager,
				const float gravity, const bool variant = false);

	EntityReq Update(const vector<Rectangle>& colliders) override;
	void Draw() override;
	void EditDraw() override;
	EntityReq OnPlayerCollision(Player& player) override;
	EntityReq OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
	bool isVariant;
	float gravity;
	Vector2 velocity{0, 0};
	float speed{0.03f};
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.18f};
	int curFrame{0};
};