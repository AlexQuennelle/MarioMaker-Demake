#include "jumpingFireEnemy.h"

#include "player.h"

#include <raylib.h>
#include <raymath.h>

JumpingFireEnemy::JumpingFireEnemy(const int x, const int y, AssetManager& assetManager, const float levelBottom)
	: Entity(x, y, assetManager), sprite(assetManager.enemies), levelBottom(levelBottom)
{
	this->solid = false;
}

EntityReq JumpingFireEnemy::Update(const vector<Rectangle>& colliders)
{
	if (this->position.y > levelBottom && accumulatedOffscreenTime < timeBetweenJumps)
	{
		accumulatedOffscreenTime += GetFrameTime();
		onScreen = false;
	}
	else if (!onScreen)
	{
		accumulatedOffscreenTime = 0;
		onScreen = true;
		this->position.y = levelBottom - 0.01f;
		this->velocity.y = -this->velocity.y * 1.01f;
	}
	else 
	{
		this->velocity.y += gravity * GetFrameTime();

		this->position = Vector2Add(position, velocity);
	}
	return {};
}
void JumpingFireEnemy::Draw()
{
	float recHeight = this->velocity.y > 0 ? -16.0f : 16.0f;

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

	Rectangle sourceRect{0.0f + (curFrame * 16.0f), 112.0f, 16.0f, recHeight};

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.y) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void JumpingFireEnemy::EditDraw()
{
	Rectangle sourceRect{0.0f, 112.0f, 16.0f, 16.0f};

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
}

EntityReq JumpingFireEnemy::OnPlayerCollision(Player& player)
{
	player.TakeDamage();

	return {};
}

EntityReq JumpingFireEnemy::OnEntityCollision(Entity& /*entity*/) { return {}; }