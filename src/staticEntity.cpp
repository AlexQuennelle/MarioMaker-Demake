#include "assetmanager.h"
#include "entity.h"
#include "player.h"

#include <cmath>
#include <raylib.h>
#include <raymath.h>

Block::Block(const int x, const int y, AssetManager& assetManager,
			 const bool variant) :
	Entity(x, y, assetManager),
	isVariant(variant),
	sprite(assetManager.staticEntities)
{
	this->solid = true;
}
auto Block::Update(const vector<Rectangle>& /*colliders*/) -> EntityReq
{
	if (this->bonkTimer > 0.0f)
	{
		bonkTimer -= GetFrameTime();
	}
	else
	{
		bonkTimer = 0.0f;
	}
	return {};
}
void Block::Draw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};
	if (this->isVariant)
		sourceRect.y += 16.0f;

	auto bonkOffset{std::sqrt(
		1.0f
		- std::pow(
			(0.5f - std::pow(this->bonkTimer / this->totalBonkTime, 1.5f))
				* 2.0f,
			2.0f))};
	bonkOffset = this->bonkTimer > 0.0f ? bonkOffset : 0.0f;
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f,
					(this->position.y * 16.0f) - (bonkOffset * 4.0f)},
				   WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.y) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void Block::EditDraw() { this->Draw(); }
auto Block::OnPlayerCollision(Player& player) -> EntityReq
{
	if (this->isVariant)
		return {};

	RecCollisionInfo info
		= GetCollisionInfo(player.GetCollisionRect(), this->GetCollider());

	if (info.minDistY < info.minDistX && info.delta.y > 0)
	{
		// head bonk
		if (player.IsBig())
			this->isActive = false;

		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();
		this->bonkTimer = this->totalBonkTime;
	}
	return {};
}
auto Block::OnEntityCollision(Entity& /*entity*/) -> EntityReq { return {}; }

Spike::Spike(const int x, const int y, AssetManager& assetManager) :
	Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = true;
	this->collider
		= {.x = 0.125f, .y = 0.125f, .width = 0.75f, .height = 0.75f};
}
auto Spike::Update(const vector<Rectangle>& /*colliders*/) -> EntityReq
{
	return {};
}
void Spike::Draw()
{
	Rectangle sourceRect{16.0f, 0.0f, 16.0f, 16.0f};
	sourceRect.y += 16.0f;

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16.0f,
					   (this->position.y + this->collider.y) * 16.0f,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void Spike::EditDraw() { this->Draw(); }
auto Spike::OnPlayerCollision(Player& player) -> EntityReq
{
	player.TakeDamage();
	return {};
}
auto Spike::OnEntityCollision(Entity& /*entity*/) -> EntityReq { return {}; }

ItemBox::ItemBox(const int x, const int y, AssetManager& assetManager,
				 const bool isBrick, const bool isHidden) :
	Entity(x, y, assetManager),
	sprite(assetManager.staticEntities),
	isBrick(isBrick),
	isHidden(isHidden)
{
	if (!this->isHidden)
		this->solid = true;
}
auto ItemBox::Update(const vector<Rectangle>& /*colliders*/) -> EntityReq
{
	if (this->bonkTimer > 0.0f)
	{
		bonkTimer -= GetFrameTime();
	}
	else
	{
		bonkTimer = 0.0f;
	}
	return {};
}
void ItemBox::Draw()
{
	if (!this->empty && this->isHidden)
		return;

	Rectangle sourceRect;
	if (!this->empty && !this->isBrick)
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
			.x = 32.0f + static_cast<float>(curFrame * 16),
			.y = 0.0f,
			.width = 16.0f,
			.height = 16.0f,
		};
	}
	else if (!this->empty && this->isBrick)
	{
		sourceRect = {.x = 0.0f, .y = 0.0f, .width = 16.0f, .height = 16.0f};
	}
	else
	{
		sourceRect = {.x = 0.0f, .y = 32.0f, .width = 16.0f, .height = 16.0f};
	}

	auto bonkOffset{std::sqrt(
		1.0f
		- std::pow(
			(0.5f - std::pow(this->bonkTimer / this->totalBonkTime, 1.5f))
				* 2.0f,
			2.0f))};
	bonkOffset = this->bonkTimer > 0.0f ? bonkOffset : 0.0f;
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f,
					(this->position.y * 16.0f) - (bonkOffset * 4.0f)},
				   WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.y) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void ItemBox::EditDraw()
{
	Rectangle sourceRect;
	if (this->isHidden)
	{
		sourceRect = {.x = 16.0f, .y = 32.0f, .width = 16.0f, .height = 16.0f};
	}
	else if (this->isBrick)
	{
		sourceRect = {.x = 0.0f, .y = 0.0f, .width = 16.0f, .height = 16.0f};
	}
	else if (!this->isBrick)
	{
		sourceRect = {.x = 32.0f, .y = 0.0f, .width = 16.0f, .height = 16.0f};
	}
	else
	{
		sourceRect = {.x = 0.0f, .y = 32.0f, .width = 16.0f, .height = 16.0f};
	}

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
}
auto ItemBox::OnPlayerCollision(Player& player) -> EntityReq
{
	if (this->empty)
		return {};

	RecCollisionInfo info
		= GetCollisionInfo(player.GetCollisionRect(), this->GetCollider());

	if (info.minDistY
		< info.minDistX
		&& info.delta.y
		> 0
		&& player.GetVelocity().y
		< 0.0f)
	{
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();

		// TODO: Add spawning of entities
		player.GainCoin();
		this->empty = true;
		this->solid = true;
		this->bonkTimer = this->totalBonkTime;
	}
	return {};
}
auto ItemBox::OnEntityCollision(Entity& /*entity*/) -> EntityReq { return {}; }

Coin::Coin(const int x, const int y, AssetManager& assetManager) :
	Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = false;
}
auto Coin::Update(const vector<Rectangle>& /*colliders*/) -> EntityReq
{
	return {};
}
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

	Rectangle sourceRect{32.0f + static_cast<float>(curFrame * 16), 16.0f,
						 16.0f, 16.0f};

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.y) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void Coin::EditDraw()
{
	Rectangle sourceRect{32.0f, 16.0f, 16.0f, 16.0f};

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
}
auto Coin::OnPlayerCollision(Player& player) -> EntityReq
{
	this->isActive = false;
	player.GainCoin();
	return {};
}
auto Coin::OnEntityCollision(Entity& /*entity*/) -> EntityReq { return {}; }

ToggleSwitch::ToggleSwitch(const int x, const int y,
						   AssetManager& assetManager) :
	Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = true;
}
auto ToggleSwitch::Update(const vector<Rectangle>& /*colliders*/) -> EntityReq
{
	if (this->bonkTimer > 0.0f)
	{
		bonkTimer -= GetFrameTime();
	}
	else
	{
		bonkTimer = 0.0f;
	}
	return {};
}
void ToggleSwitch::Draw()
{
	Rectangle sourceRect{0.0f, 48.0f, 16.0f, 16.0f};
	if (this->on)
		sourceRect.y += 16.0f;

	auto bonkOffset{std::sqrt(
		1.0f
		- std::pow(
			(0.5f - std::pow(this->bonkTimer / this->totalBonkTime, 1.5f))
				* 2.0f,
			2.0f))};
	bonkOffset = this->bonkTimer > 0.0f ? bonkOffset : 0.0f;
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f,
					(this->position.y * 16.0f) - (bonkOffset * 4.0f)},
				   WHITE);

#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.y) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void ToggleSwitch::EditDraw() { this->Draw(); }
auto ToggleSwitch::OnPlayerCollision(Player& player) -> EntityReq
{
	RecCollisionInfo info
		= GetCollisionInfo(player.GetCollisionRect(), this->GetCollider());

	if (info.minDistY < info.minDistX && info.delta.y > 0)
	{
		// head bonk
		this->on = !this->on;
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();
		this->bonkTimer = this->totalBonkTime;
		return {ToggleRequest()};
	}
	return {};
}
auto ToggleSwitch::OnEntityCollision(Entity& /*entity*/) -> EntityReq
{
	return {};
}

ToggleBlock::ToggleBlock(const int x, const int y, AssetManager& assetManager,
						 const bool startOn) :
	Entity(x, y, assetManager),
	startOn(startOn),
	on(false),
	sprite(assetManager.staticEntities)
{
	if (!this->startOn)
		this->solid = true;
}
auto ToggleBlock::Update(const vector<Rectangle>& /*colliders*/) -> EntityReq
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
					   (this->position.y + this->collider.y) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void ToggleBlock::EditDraw() { this->Draw(); }
auto ToggleBlock::OnEntityCollision(Entity& /*entity*/) -> EntityReq
{
	return {};
};
auto ToggleBlock::OnPlayerCollision(Player& /*player*/) -> EntityReq
{
	return {};
};
