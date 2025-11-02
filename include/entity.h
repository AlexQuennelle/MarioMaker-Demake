#pragma once

#include "assetmanager.h"
#include "raylib.h"

class Player;

class Entity
{
	public:
	Entity(const int x, const int y /*, Texture& sprites*/);

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void OnPlayerCollision(Player& player) = 0;
	virtual void OnEntityCollision(Entity& entity) = 0;

	virtual bool IsSolid() const = 0;
	Rectangle GetCollider() const { 
		return {.x = this->collider.x + this->position.x,
				.y = this->collider.y + this->position.y,
				.width = this->collider.width,
				.height = this->collider.height};
	};

	protected:
	bool isActive{true};
	// must be set in child constructors, might be kinda hacky
	bool solid;
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
	void OnPlayerCollision(Player& player) override {};
	void OnEntityCollision(Entity& entity) override {};
	bool IsSolid() const override { return this->solid; }

	private:
};
