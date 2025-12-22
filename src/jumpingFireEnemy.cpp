#include "jumpingFireEnemy.h"

#include "player.h"

#include <cmath>
#include <raylib.h>
#include <raymath.h>

JumpingFireEnemy::JumpingFireEnemy(const int x, const int y,
								   AssetManager& assetManager,
								   const float levelBottom) :
	Entity(x, y, assetManager),
	sprite(assetManager.enemies),
	jumpDestination(this->position.y),
	levelBottom(levelBottom)
{
	this->solid = false;
}

EntityReq JumpingFireEnemy::Update(const vector<Rectangle>& /*colliders*/)
{
	if (this->position.y
		>= levelBottom
		&& accumulatedOffscreenTime
		< timeBetweenJumps)
	{
		accumulatedOffscreenTime += GetFrameTime();
		onScreen = false;
	}
	else if (!onScreen)
	{
		accumulatedOffscreenTime = 0;
		accumulatedJumpTime = 0;
		onScreen = true;
		this->position.y -= 0.02f;
	}
	else
	{
		accumulatedJumpTime += GetFrameTime();
		accumulatedJumpTime = Clamp(accumulatedJumpTime, 0, jumpDuration);
		if (falling)
		{
			float t = pow(accumulatedJumpTime / jumpDuration, 2);

			this->position.y = std::lerp(jumpDestination, levelBottom, t);
		}
		else
		{
			float t = 1 - pow(1 - (accumulatedJumpTime / jumpDuration), 2);

			this->position.y = std::lerp(levelBottom, jumpDestination, t);
		}
		if (accumulatedJumpTime >= jumpDuration)
		{
			accumulatedJumpTime = 0;
			falling = !falling;
		}
	}
	return {};
}
void JumpingFireEnemy::Draw()
{
	float recHeight = this->falling ? -16.0f : 16.0f;

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
