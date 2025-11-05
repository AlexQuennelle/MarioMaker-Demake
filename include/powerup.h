#pragma once

#include "entity.h"

class Mushroom : public Entity
{
	public:
	Mushroom(const int x, const int y, AssetManager& assetManager, float gravity);

	void Update(const vector<Rectangle>& colliders) override;
	void Draw() override;
	void OnPlayerCollision(Player& player) override;
	void OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
	Vector2 velocity{0, 0};
	float speed{0.05f};
	float gravity;
};