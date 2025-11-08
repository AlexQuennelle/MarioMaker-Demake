#include "walkerEnemy.h"
#include "player.h"

WalkerEnemy::WalkerEnemy(const int x, const int y, AssetManager& assetManager, const float gravity, const bool variant)
	: Entity(x, y, assetManager), sprite(assetManager.enemies),
	  gravity(gravity), isVariant(variant)
{
	this->solid = false;
	this->collider = {0.0f, 0.3f, 1.0f, 0.7f};
	this->velocity.x = -this->speed;
}

EntityReq WalkerEnemy::Update(const vector<Rectangle>& colliders)
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
				if ((info.delta.x <= 0 || velocity.x <= 0) &&
					(info.delta.x >= 0 || velocity.x >= 0))
				{
					this->velocity.x =
						copysignf(this->speed, -this->velocity.x);
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
void WalkerEnemy::Draw()
{
	float recWidth = this->velocity.x > 0 ? -16 : 16;

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

	Rectangle sourceRect{0.0f + (curFrame * 16.0f), 0.0f, recWidth, 16.0f};

	if (this->isVariant)
	{
		sourceRect.y += 16;
	}

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines((this->position.x + this->collider.x) * 16,
					   (this->position.y + this->collider.y) * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void WalkerEnemy::EditDraw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};

	if (this->isVariant)
	{
		sourceRect.y += 16;
	}

	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
}

EntityReq WalkerEnemy::OnPlayerCollision(Player& player)
{
	if (player.GetPosition().y < this->position.y + 0.5f)
	{
		// squash
		this->isActive = false;

		float bounceForce = player.IsJumpPressed() ? -0.4f : -0.2f;
		player.SetVelocity({player.GetVelocity().x, bounceForce});
	}
	else
	{
		player.TakeDamage();
	}

	return {};
}

EntityReq WalkerEnemy::OnEntityCollision(Entity& /*entity*/) { return {}; }