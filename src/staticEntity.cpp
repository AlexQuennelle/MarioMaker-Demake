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
EntityReq Brick::Update(const vector<Rectangle>& /*colliders*/) { return {}; }
void Brick::Draw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};
	if (this->isVariant)
		sourceRect.y += 16.0f;

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.x) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
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

Spike::Spike(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = true;
	this->collider = {
		.x = 0.125f, .y = 0.125f, .width = 0.75f, .height = 0.75f};
}
EntityReq Spike::Update(const vector<Rectangle>& /*colliders*/) { return {}; }
void Spike::Draw()
{
	Rectangle sourceRect{16.0f, 0.0f, 16.0f, 16.0f};
	sourceRect.y += 16.0f;

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16.0f,
					   (this->position.y + this->collider.x) * 16.0f,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
EntityReq Spike::OnPlayerCollision(Player& player)
{
	player.TakeDamage();
	return {};
}
EntityReq Spike::OnEntityCollision(Entity& /*entity*/) { return {}; }

ItemBox::ItemBox(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = true;
}

EntityReq ItemBox::Update(const vector<Rectangle>& /*colliders*/) { return {}; }
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
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.x) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
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
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();

		// TODO: Add spawning of entities
		player.GainCoin();
		this->empty = true;
	}
	return {};
}
EntityReq ItemBox::OnEntityCollision(Entity& /*entity*/) { return {}; }

Coin::Coin(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = false;
}
EntityReq Coin::Update(const vector<Rectangle>& /*colliders*/) { return {}; }
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
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.x) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
EntityReq Coin::OnPlayerCollision(Player& player)
{
	this->isActive = false;
	player.GainCoin();
	return {};
}
EntityReq Coin::OnEntityCollision(Entity& /*entity*/) { return {}; }

ToggleSwitch::ToggleSwitch(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = true;
}
EntityReq ToggleSwitch::Update(const vector<Rectangle>& /*colliders*/)
{
	return {};
}
void ToggleSwitch::Draw()
{
	Rectangle sourceRect{0.0f, 48.0f, 16.0f, 16.0f};
	if (this->on)
		sourceRect.y += 16.0f;

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.x) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
EntityReq ToggleSwitch::OnPlayerCollision(Player& player)
{
	RecCollisionInfo info =
		GetCollisionInfo(player.GetCollisionRect(), this->GetCollider());

	if (info.minDistY < info.minDistX && info.delta.y > 0)
	{
		// head bonk
		this->on = !this->on;
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();
		return {ToggleRequest()};
	}
	return {};
}
EntityReq ToggleSwitch::OnEntityCollision(Entity& /*entity*/) { return {}; }

ToggleBlock::ToggleBlock(const int x, const int y, AssetManager& assetManager,
						 const bool startOn)
	: Entity(x, y, assetManager), startOn(startOn), on(false),
	  sprite(assetManager.staticEntities)
{
	if (!this->startOn)
		this->solid = true;
}
EntityReq ToggleBlock::Update(const vector<Rectangle>& /*colliders*/)
{
	this->solid = (!on) ^ this->startOn;
	return {};
}
void ToggleBlock::Draw()
{
	Rectangle sourceRect{32.0f, 48.0f, 16.0f, 16.0f};
	if (this->startOn)
		sourceRect.y += 16.0f;
	if ((this->on) ^ this->startOn)
		sourceRect.x += 16.0f;

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.x) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
