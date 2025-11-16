#pragma once

#include "entity.h"

#include <raylib.h>
#include <vector>

class Fireball
{
	public:
	Fireball(Texture2D& sprite, bool facingRight, Vector2 startingPos);
	void Update(std::vector<Entity*> entities, std::vector<Rectangle> solidCols);
	void Draw();
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

	private:
	Rectangle collider{0.0f, 0.0f, 0.5f, 0.5f};
	Vector2 position;
	Texture2D& sprite;
	Vector2 velocity{0, 0};
	float speed{0.2f};
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.12f};
	float lifetime{5.0f};
	float gravity{1.3f};
	int curFrame{0};
	bool isActive{true};
};