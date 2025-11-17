#pragma once

#include "entity.h"

class JumpingFireEnemy : public Entity
{
	public:
	JumpingFireEnemy(const int x, const int y, AssetManager& assetManager,
					 const float levelBottom);

	EntityReq Update(const vector<Rectangle>& colliders) override;
	void Draw() override;
	void EditDraw() override;
	EntityReq OnPlayerCollision(Player& player) override;
	EntityReq OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
	Vector2 velocity{0, 0};
	// tweakable gravity because it doesnt interact with the world like everything else
	float gravity{0.6f};
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.18f};
	int curFrame{0};
	bool onScreen{true};
	float timeBetweenJumps{1.0f};
	float accumulatedOffscreenTime{0.0f};
	float levelBottom;
};