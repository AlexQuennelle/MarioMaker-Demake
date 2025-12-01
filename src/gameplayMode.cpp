#include "assetmanager.h"
#include "gamemode.h"

#include <format>
#include <raylib.h>
#include <raymath.h>

GameplayMode::GameplayMode(Level* lvl, AssetManager& am) :
	GamemodeInstance(lvl, am),
	player(*lvl, {this->assetManager.playerSprites,
				  this->assetManager.staticEntities}),
	inputHandler(this->player),
	time(300),
	uiDisplay(this->assetManager.smallFont)
{
	this->camera = Camera2D{0};
	this->camera.target = this->player.GetPosition();
	// Change offset to adjust relative position
	this->camera.offset = {.x = 192.0f, .y = 124.0f};
	this->camera.rotation = 0.0f;
	this->camera.zoom = 1.0f;

	// HACK: change to read from level soon

	player.Reset(level->GetPlayerStartPos());
}
void GameplayMode::Update()
{
	// HACK: Temporary escape to main menu
	if (IsKeyPressed(KEY_BACKSPACE))
		this->switchReq = SwitchRequest::MainMenu;

	// input gets polled first
	inputHandler.Update();

	// skip all updates if game is paused
	if (inputHandler.IsPaused())
		return;

	player.AddForce({0, gravity * GetFrameTime()});
	player.Update();

	// auauguyhh
	if (player.GetPosition().x < 12)
	{
		this->camera.offset.x = 192.0f - ((12 - player.GetPosition().x) * 16);
	}
	else if (player.GetPosition().x > level->GetLength() - 12)
	{
		this->camera.offset.x
			= 192.0f
			  + ((player.GetPosition().x - level->GetLength() + 12) * 16);
	}
	// TODO: Add top checking
	if (player.GetPosition().y > level->GetHeight() - 6.0f)
	{
		this->camera.offset.y
			= 124.0f
			  + ((6.75f
				  - (level->GetHeight() - (player.GetPosition().y - 1.0f)))
				 * 16.0f);
	}
	else
	{
		this->camera.offset.y = 124;
	}

	if (player.IsDead())
	{
		timeDead += GetFrameTime();
	}
	if (timeDead >= 1.5f)
	{
		this->Reset();
	}

	if (time <= 0)
	{
		time = 0;
		player.Die();
	}

	time -= GetFrameTime();

	this->level->Update();

	// Update camera target to player position
	this->camera.target = this->player.GetPosition() * 16.0f;
}
void GameplayMode::Draw()
{
	BeginMode2D(this->camera);
	this->level->Draw();
	this->player.Draw();
	EndMode2D();
}
void GameplayMode::DrawUI()
{
	this->uiDisplay.Draw(this->time, this->player.GetCoins());
}

void GameplayMode::Reset()
{
	std::cout << "reset\n";
	level->Reset();
	player.Reset(level->GetPlayerStartPos());
	timeDead = 0;

	// HACK: change to read from level soon
	time = 300;
}
