#include "powerup.h"
#include "assetmanager.h"
#include "entity.h"
#include "player.h"

#include <raylib.h>
#include <raymath.h>

Mushroom::Mushroom(const int x, const int y, AssetManager& assetManager,
				   const float gravity) :
	Entity(x, y, assetManager), sprite(assetManager.powerups), gravity(gravity)
{
	this->solid = false;
	this->velocity.x = this->speed;
}

auto Mushroom::Update(const vector<Rectangle>& colliders) -> EntityReq
{
	this->velocity.y += gravity * GetFrameTime();

	this->position = Vector2Add(position, velocity);

	for (const Rectangle col : colliders)
	{
		if (CheckCollisionRecs(this->GetCollider(), col))
		{
			RecCollisionInfo info = GetCollisionInfo(this->GetCollider(), col);
			if (info.minDistX < info.minDistY)
			{
				this->position.x += copysignf(info.minDistX, info.delta.x);
				if ((info.delta.x <= 0 || velocity.x <= 0)
					&& (info.delta.x >= 0 || velocity.x >= 0))
				{
					this->velocity.x
						= copysignf(this->speed, -this->velocity.x);
				}
			}
			else
			{
				this->position.y += copysignf(info.minDistY, info.delta.y);
				this->velocity.y = 0;
			}
		}
	}
	return {};
}
void Mushroom::Draw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x * 16, this->position.y * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void Mushroom::EditDraw() { this->Draw(); }

auto Mushroom::OnPlayerCollision(Player& player) -> EntityReq
{
	player.GetBig();
	this->isActive = false;
	return {};
}

auto Mushroom::OnEntityCollision(Entity& /*entity*/) -> EntityReq { return {}; }

FireFlower::FireFlower(const int x, const int y, AssetManager& assetManager,
					   const float gravity) :
	Entity(x, y, assetManager), sprite(assetManager.powerups), gravity(gravity)
{
	this->solid = false;
}

auto FireFlower::Update(const vector<Rectangle>& colliders) -> EntityReq
{
	this->velocity.y += gravity * GetFrameTime();

	this->position = Vector2Add(position, velocity);

	for (const Rectangle col : colliders)
	{
		if (CheckCollisionRecs(this->GetCollider(), col))
		{
			RecCollisionInfo info = GetCollisionInfo(this->GetCollider(), col);
			if (info.minDistX < info.minDistY)
			{
				this->position.x += copysignf(info.minDistX, info.delta.x);
			}
			else
			{
				this->position.y += copysignf(info.minDistY, info.delta.y);
				this->velocity.y = 0;
			}
		}
	}
	return {};
}

void FireFlower::Draw()
{
	if (accumulatedAnimTime >= timeBetweenFrames)
	{
		accumulatedAnimTime = 0;
		curFrame++;
	}
	if (curFrame > 1)
	{
		curFrame = 0;
	}

	accumulatedAnimTime += GetFrameTime();

	Rectangle sourceRect{32.0f + static_cast<float>(curFrame * 16), 0.0f, 16.0f,
						 16.0f};
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x * 16, this->position.y * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void FireFlower::EditDraw()
{
	Rectangle sourceRect{32.0f, 0.0f, 16.0f, 16.0f};
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x * 16, this->position.y * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}

auto FireFlower::OnPlayerCollision(Player& player) -> EntityReq
{
	player.GetFire();
	this->isActive = false;
	return {};
}

auto FireFlower::OnEntityCollision(Entity& /*entity*/) -> EntityReq
{
	return {};
}
