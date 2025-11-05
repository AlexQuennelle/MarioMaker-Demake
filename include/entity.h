#pragma once

#include "assetmanager.h"
#include "raylib.h"
#include "raymath.h"
#include <vector>

using std::vector;

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

	virtual void Update(const vector<Rectangle>& colliders) = 0;
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
	bool solid{false};
	Vector2 position;
	Rectangle collider{0.0f, 0.0f, 1.0f, 1.0f};
	AssetManager& assetManager;
};

class Brick : public Entity
{
	public:
	Brick(const int x, const int y, AssetManager& assetManager);

	void Update(const vector<Rectangle>& colliders) override;
	void Draw() override;
	void OnPlayerCollision(Player& player) override;
	void OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
};

class Coin : public Entity
{
	public:
	Coin(const int x, const int y, AssetManager& assetmanager);

	void Update(const vector<Rectangle>& colliders) override;
	void Draw() override;
	void OnPlayerCollision(Player& player) override;
	void OnEntityCollision(Entity& entity) override;

	private:
	Texture2D& sprite;
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.12f};
	int curFrame{0};
};
