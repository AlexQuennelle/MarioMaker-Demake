#pragma once

#include "entity.h"

class WalkerEnemy : public Entity
{
	public:
	WalkerEnemy(const int x, const int y, AssetManager& assetManager,
				const float gravity);

	EntityReq Update(const vector<Rectangle>& colliders) override;
	void Draw() override;
	void EditDraw() override;
	EntityReq OnPlayerCollision(Player& player) override;
	EntityReq OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
	float gravity;
};