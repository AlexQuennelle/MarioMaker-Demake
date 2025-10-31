#include "gamemode.h"
#include "assetmanager.h"

#include <raylib.h>
#include <raymath.h>

GameplayMode::GameplayMode(Level& lvl, asset_ptr& am)
	: GamemodeInstance(lvl, am),
	  player(lvl, {this->assetManager->playerSprites}),
	  inputHandler(this->player)
{
	this->camera = Camera2D{0};
	this->camera.target = this->player.GetPosition();
	// Change offset to adjust relative position
	this->camera.offset = {.x = 192.0f, .y = 108.0f};
	this->camera.rotation = 0.0f;
	this->camera.zoom = 1.0f;

	player.Reset(level.GetPlayerStartPos());
}
void GameplayMode::Update()
{
	// input gets polled first
	inputHandler.Update();

	// skip all updates if game is paused
	if (inputHandler.IsPaused())
		return;

	player.AddForce({0, gravity * GetFrameTime()});
	player.Update();

	if (player.IsDead())
	{
		timeDead += GetFrameTime();
	}
	if (timeDead >= 1.5f)
	{
		this->Reset();
	}

	this->level.Update();

	// Update camera target to player position
	this->camera.target = this->player.GetPosition() * 16.0f;
}
void GameplayMode::Draw()
{
	BeginMode2D(this->camera);
	this->level.Draw();
	this->player.Draw();
	EndMode2D();
}
void GameplayMode::DrawUI() {}
void GameplayMode::Reset()
{
	level.Reset();
	player.Reset(level.GetPlayerStartPos());
	timeDead = 0;
}
