#include "gamemode.h"
#include "assetmanager.h"

#include <raylib.h>

GameplayMode::GameplayMode(Level& lvl, asset_ptr& am)
	: GamemodeInstance(lvl, am),
	  player(lvl, {this->assetManager->playerSprites}),
	  inputHandler(this->player)
{
	this->camera = Camera2D{0};
	this->camera.target = {.x = 0.0f, .y = 0.0f};
	this->camera.offset = {.x = 0.0f, .y = 0.0f};
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
}
void GameplayMode::Draw()
{
	this->level.Draw();
	this->player.Draw();
}
void GameplayMode::DrawUI() {}
void GameplayMode::Reset()
{
	level.Reset();
	player.Reset(level.GetPlayerStartPos());
	timeDead = 0;
}
