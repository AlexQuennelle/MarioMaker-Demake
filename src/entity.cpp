#include "entity.h"
#include "assetmanager.h"

#include <raylib.h>

Entity::Entity(const int x, const int y /*, Texture& sprites*/)
	: position(x, y) /* sprites(sprites)*/
{}

Brick::Brick(const int x, const int y, const AssetManager& assetManager)
	: Entity(x, y)
{
	// TODO: Proper initialization
}
void Brick::Update() {}
void Brick::Draw()
{
	// HACK: Temp square for testing
	DrawRectangleRec(
		{this->position.x * 16.0f, this->position.y * 16.0f, 16.0f, 16.0f},
		BROWN);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x, this->position.y, this->collider.width,
					   this->collider.height, Fade(RED, 0.6f));
#endif // DEBUG
}
bool Brick::Bonk(const bool isPlayer, const bool big) { return true; }
