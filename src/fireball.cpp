#include "fireball.h"

#include <raymath.h>
#include <vector>

using std::vector;

Fireball::Fireball(Texture2D& sprite, bool facingRight, Vector2 startingPos) :
	position(startingPos), sprite(sprite)
{
	this->velocity.x = facingRight ? speed : -speed;
}

void Fireball::Update(const vector<Entity*>& entities,
					  const vector<Rectangle>& solidCols)
{
	if (!this->isActive)
		return;

	this->velocity.y += gravity * GetFrameTime();

	this->position = Vector2Add(this->position, this->velocity);

	for (const auto& entity : entities)
	{
		if (CheckCollisionRecs(this->GetCollider(), entity->GetCollider()))
		{
			if (entity->TakeDamage())
			{
				this->isActive = false;
				return;
			}
		}
	}

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

	Rectangle sourceRect{48.0f + static_cast<float>(curFrame * 16), 32.0f,
						 16.0f, 16.0f};

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
}
auto Fireball::IsActive() const -> bool { return this->isActive; }
auto Fireball::GetCollider() const -> Rectangle
{
	return {
		.x = this->collider.x + this->position.x,
		.y = this->collider.y + this->position.y,
		.width = this->collider.width,
		.height = this->collider.height,
	};
};
