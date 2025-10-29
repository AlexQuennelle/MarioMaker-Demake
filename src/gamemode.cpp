#include "gamemode.h"

void GameplayMode::Update()
{
	// input gets polled first :craigthumb:
	inputHandler.Update();

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
void GameplayMode::Reset()
{
	level.Reset();
	player.Reset(level.GetPlayerStartPos());
	timeDead = 0;
}
