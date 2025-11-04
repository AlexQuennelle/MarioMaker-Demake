#include "entity.h"
#include "assetmanager.h"
#include "player.h"

#include <raylib.h>
#include <raymath.h>

Entity::Entity(const int x, const int y, AssetManager& assetManager)
	: position(x, y), assetManager(assetManager)
{}

Brick::Brick(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	// TODO: Proper initialization
}
void Brick::Update() {}
void Brick::Draw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x, this->position.y, this->collider.width,
					   this->collider.height, Fade(RED, 0.6f));
#endif // DEBUG
}

void Brick::OnPlayerCollision(Player& player)
{

	Rectangle playerCol{player.GetCollisionRect()};
	Rectangle col{this->GetCollider()};

	const Vector2 center1 = {playerCol.x + (playerCol.width / 2),
							 playerCol.y + (playerCol.height / 2)};
	const Vector2 center2 = {col.x + (col.width / 2), col.y + (col.height / 2)};

	const Vector2 delta = Vector2Subtract(center1, center2);

	const Vector2 hs1 = {playerCol.width * .5f, playerCol.height * .5f};
	const Vector2 hs2 = {col.width * .5f, col.height * .5f};

	const float minDistX = hs1.x + hs2.x - fabsf(delta.x);
	const float minDistY = hs1.y + hs2.y - fabsf(delta.y);

	if (minDistY < minDistX && delta.y > 0)
	{
		// head bonk
		this->isActive = false;
		player.SetVelocity({player.GetVelocity().x, 0});
		player.CancelJump();
	}
}

void Brick::OnEntityCollision(Entity& entity) {}
