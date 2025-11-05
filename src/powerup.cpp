#include "powerup.h"
#include "assetmanager.h"
#include "player.h"

#include <raylib.h>
#include <raymath.h>


Mushroom::Mushroom(const int x, const int y, AssetManager& assetManager, float gravity)
	: Entity(x, y, assetManager), sprite(assetManager.powerups), gravity(gravity)
{
	this->solid = false;
	this->velocity.x = this->speed;
}


void Mushroom::Update(const vector<Rectangle>& colliders) 
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
				if (!(info.delta.x > 0 && velocity.x > 0) &&
					!(info.delta.x < 0 && velocity.x < 0))
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
}
void Mushroom::Draw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x, this->position.y, this->collider.width,
					   this->collider.height, Fade(RED, 0.6f));
#endif // DEBUG
}

void Mushroom::OnPlayerCollision(Player& player)
{ 
	player.GetBig(); 
	this->isActive = false;
}

void Mushroom::OnEntityCollision(Entity& entity) {}