#pragma once

#include "entity.h"

class Mushroom : public Entity
{
	public:
	Mushroom(const int x, const int y, AssetManager& assetManager,
			 float gravity);

	EntityReq Update(const vector<Rectangle>& colliders) override;
	void Draw() override;
	void EditDraw() override;
	EntityReq OnPlayerCollision(Player& player) override;
	EntityReq OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
	Vector2 velocity{0, 0};
	float speed{0.05f};
	float gravity;
};
