#include "fireball.h"

#include <iostream>
#include <raymath.h>

Fireball::Fireball(Texture2D& sprite, bool facingRight, Vector2 startingPos)
	: sprite(sprite), position(startingPos)
{
	this->velocity.x = facingRight ? speed : -speed;
}

void Fireball::Update(std::vector<Entity*> entities,
	std::vector<Rectangle> solidCols)
{
	if (!this->isActive)
		return;

	this->velocity.y += gravity * GetFrameTime();

	this->position = Vector2Add(this->position, this->velocity);

	for (const Rectangle col : solidCols)
	{
		if (CheckCollisionRecs(this->GetCollider(), col))
		{
			RecCollisionInfo info = GetCollisionInfo(this->GetCollider(), col);
			if (info.minDistX < info.minDistY)
			{
				this->isActive = false;
				return;
			}
			else
			{
				this->position.y += copysignf(info.minDistY, info.delta.y);
				this->velocity.y = -0.3f;
			}
		}
	}

	this->lifetime -= GetFrameTime();

	this->isActive = this->lifetime > 0;
}

void Fireball::Draw() 
{
	if (accumulatedAnimTime >= timeBetweenFrames)
	{
		accumulatedAnimTime = 0;
		curFrame++;
	}
	if (curFrame > 2)
	{
		curFrame = 0;
	}

	accumulatedAnimTime += GetFrameTime();

	Rectangle sourceRect{48.0f + (curFrame * 16.0f), 32.0f, 16.0f, 16.0f};

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
}