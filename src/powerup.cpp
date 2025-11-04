#include "powerup.h"
#include "assetmanager.h"
#include "player.h"

#include <raylib.h>
#include <raymath.h>


Mushroom::Mushroom(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.powerups)
{
	this->solid = false;
}


void Mushroom::Update() {}
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
	RecCollisionInfo info =
		GetCollisionInfo(player.GetCollisionRect(), this->GetCollider());

	if (player.IsBig() && info.minDistY < info.minDistX && info.delta.y > 0)
	{
		// head bonk
		this->isActive = false;
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();
	}
}

void Mushroom::OnEntityCollision(Entity& entity) {}