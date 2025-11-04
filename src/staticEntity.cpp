#include "assetmanager.h"
#include "entity.h"
#include "player.h"

#include <raylib.h>
#include <raymath.h>

Brick::Brick(const int x, const int y, AssetManager& assetManager,
			 const bool variant)
	: Entity(x, y, assetManager), isVariant(variant),
	  sprite(assetManager.staticEntities)
{
	this->solid = true;
}
EntityReq Brick::Update() { return {}; }
void Brick::Draw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};
	if (this->isVariant)
		sourceRect.y += 16.0f;

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x, this->position.y, this->collider.width,
					   this->collider.height, Fade(RED, 0.6f));
#endif // DEBUG
}
EntityReq Brick::OnPlayerCollision(Player& player)
{
	if (this->isVariant)
		return {};

	RecCollisionInfo info =
		GetCollisionInfo(player.GetCollisionRect(), this->GetCollider());

	if (player.IsBig() && info.minDistY < info.minDistX && info.delta.y > 0)
	{
		// head bonk
		this->isActive = false;
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();
	}
	return {};
}
EntityReq Brick::OnEntityCollision(Entity& /*entity*/) { return {}; }

Coin::Coin(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = false;
}
EntityReq Coin::Update() { return {}; }
void Coin::Draw()
{

	if (accumulatedAnimTime >= timeBetweenFrames)
	{
		accumulatedAnimTime = 0;
		curFrame++;
	}
	if (curFrame > 3)
	{
		curFrame = 0;
	}

	accumulatedAnimTime += GetFrameTime();

	Rectangle sourceRect{32.0f + (curFrame * 16.0f), 16.0f, 16.0f, 16.0f};

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x, this->position.y, this->collider.width,
					   this->collider.height, Fade(RED, 0.6f));
#endif // DEBUG
}
EntityReq Coin::OnPlayerCollision(Player& player)
{
	this->isActive = false;
	player.GainCoin();
	return {};
}
EntityReq Coin::OnEntityCollision(Entity& /*entity*/) { return {}; }

ItemBox::ItemBox(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = true;
}
EntityReq ItemBox::Update() { return {}; }
void ItemBox::Draw()
{
	Rectangle sourceRect;
	if (!empty)
	{
		if (accumulatedAnimTime >= timeBetweenFrames)
		{
			accumulatedAnimTime = 0;
			curFrame++;
		}
		if (curFrame > 3)
		{
			curFrame = 0;
		}

		accumulatedAnimTime += GetFrameTime();
		sourceRect = {
			.x = 32.0f + (curFrame * 16.0f),
			.y = 0.0f,
			.width = 16.0f,
			.height = 16.0f,
		};
	}
	else
	{
		sourceRect = {.x = 0.0f, .y = 32.0f, .width = 16.0f, .height = 16.0f};
	}

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x, this->position.y, this->collider.width,
					   this->collider.height, Fade(RED, 0.6f));
#endif // DEBUG
}
EntityReq ItemBox::OnPlayerCollision(Player& player)
{
	if (this->empty)
		return {};

	RecCollisionInfo info =
		GetCollisionInfo(player.GetCollisionRect(), this->GetCollider());

	if (info.minDistY < info.minDistX && info.delta.y > 0)
	{
		// head bonk
		//this->isActive = false;
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();

		// TODO: Add spawning of entities
		player.GainCoin();
		this->empty = true;
	}
	return {};
}
EntityReq ItemBox::OnEntityCollision(Entity& /*entity*/) { return {}; }
