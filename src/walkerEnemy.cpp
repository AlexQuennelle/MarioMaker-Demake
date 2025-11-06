#include "walkerEnemy.h"

WalkerEnemy::WalkerEnemy(const int x, const int y, AssetManager& assetManager, const float gravity)
	: Entity(x, y, assetManager), sprite(assetManager.staticEntities), gravity(gravity)
{
	this->solid = false;
}

EntityReq WalkerEnemy::Update(const vector<Rectangle>& colliders)
{
	
	return {};
}
void WalkerEnemy::Draw()
{
	Rectangle sourceRect{0.0f, 0.0f, 16.0f, 16.0f};
	DrawTextureRec(this->sprite, sourceRect,
				   {this->position.x * 16.0f, this->position.y * 16.0f}, WHITE);
#ifdef DRAW_COLS
	DrawRectangleLines(this->position.x * 16, this->position.y * 16,
					   this->collider.width * 16, this->collider.height * 16,
					   Fade(RED, 0.6f));
#endif // DEBUG
}
void WalkerEnemy::EditDraw() { this->Draw(); }

EntityReq WalkerEnemy::OnPlayerCollision(Player& player)
{
	
	return {};
}

EntityReq WalkerEnemy::OnEntityCollision(Entity& /*entity*/) { return {}; }