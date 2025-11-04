#pragma once

#include "assetmanager.h"
#include "raylib.h"
#include "raymath.h"

class Player;

// collision utilities
struct RecCollisionInfo
{
	public:
	Vector2 delta;
	float minDistX;
	float minDistY;
};

RecCollisionInfo GetCollisionInfo(Rectangle, Rectangle);

class Entity
{
	public:
	Entity(const int x, const int y, AssetManager& assetManager);
	virtual ~Entity() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void OnPlayerCollision(Player& player) = 0;
	virtual void OnEntityCollision(Entity& entity) = 0;

	bool IsSolid() const { return this->solid; }
	bool IsActive() const { return this->isActive; }
	Rectangle GetCollider() const
	{
		return {
			.x = this->collider.x + this->position.x,
			.y = this->collider.y + this->position.y,
			.width = this->collider.width,
			.height = this->collider.height,
		};
	};

	protected:
	bool isActive{true};
	bool solid{true};
	Vector2 position;
	Rectangle collider{0.0f, 0.0f, 1.0f, 1.0f};
	AssetManager& assetManager;
};

class Brick : public Entity
{
	public:
	Brick(const int x, const int y, AssetManager& assetManager);

	void Update() override;
	void Draw() override;
	void OnPlayerCollision(Player& player) override;
	void OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
};
