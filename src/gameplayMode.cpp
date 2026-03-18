#include "assetmanager.h"
#include "gamemode.h"

#include <raylib.h>
#include <raymath.h>

GameplayMode::GameplayMode(Level* lvl, AssetManager& am) :
	GamemodeInstance(lvl, am),
	player(*lvl, {this->assetManager.playerSprites,
				  this->assetManager.staticEntities}),
	uiDisplay(this->assetManager.smallFont),
	inputHandler(this->player),
	time(300)
{
	this->camera = Camera2D{
		{.x = 192.0f, .y = 124.0f},
		this->player.GetPosition(), // HACK: Change to read from level soon
		0.0f,
		1.0f,
	};

	// HACK: Change to read from level soon
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
	else if (player.GetPosition().x
			 > static_cast<float>(level->GetLength() - 12))
	{
		this->camera.offset.x = 192.0f
								+ ((player.GetPosition().x
									- static_cast<float>(level->GetLength())
									+ 12.0f)
								   * 16.0f);
	}
	// TODO: Add top checking
	if (player.GetPosition().y > static_cast<float>(level->GetHeight()) - 6.0f)
	{
		this->camera.offset.y = 124.0f
								+ ((6.75f
									- (static_cast<float>(level->GetHeight())
									   - (player.GetPosition().y - 1.0f)))
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
