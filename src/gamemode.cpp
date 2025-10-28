#include "gamemode.h"

#include <raylib.h>

GameplayMode::GameplayMode(Level& lvl)
	: GamemodeInstance(lvl), player(lvl), inputHandler(this->player)
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
	// input gets polled first :craigthumb:
	inputHandler.Update();

	player.AddForce({0, gravity * GetFrameTime()});
	player.Update();
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
}
