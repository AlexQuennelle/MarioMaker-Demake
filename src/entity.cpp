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
	this->solid = true;
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

void Brick::OnEntityCollision(Entity& entity) {}

RecCollisionInfo GetCollisionInfo(Rectangle col1, Rectangle col2)
{
	// Calculation of centers of rectangles
	const Vector2 center1 = {col1.x + (col1.width / 2),
							 col1.y + (col1.height / 2)};
	const Vector2 center2 = {col2.x + (col2.width / 2),
							 col2.y + (col2.height / 2)};

	// Calculation of the distance vector between the centers of the
	// rectangles
	const Vector2 delta = Vector2Subtract(center1, center2);

	// Calculation of half-widths and half-heights of rectangles
	const Vector2 hs1 = {col1.width * .5f, col1.height * .5f};
	const Vector2 hs2 = {col2.width * .5f, col2.height * .5f};

	// Calculation of the minimum distance at which the two rectangles
	// can be separated
	const float minDistX = hs1.x + hs2.x - fabsf(delta.x);
	const float minDistY = hs1.y + hs2.y - fabsf(delta.y);

	return {.delta = delta, .minDistX = minDistX, .minDistY = minDistY};
};

Coin::Coin(const int x, const int y, AssetManager& assetManager)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities)
{
	this->solid = false;
}

void Coin::Update() {}

void Coin::Draw() {
	

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

void Coin::OnPlayerCollision(Player& player)
{ 
	this->isActive = false;
	player.GainCoin();
}

void Coin::OnEntityCollision(Entity& entity) {}