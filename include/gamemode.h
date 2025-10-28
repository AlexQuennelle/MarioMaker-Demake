#pragma once

#include "level.h"
#include "player.h"
#include "playerInputHandler.h"
#include "assetmanager.h"

class GamemodeInstance
{
	public:
	GamemodeInstance(Level& lvl, asset_ptr& am) : level(lvl), assetManager(am) {};
	virtual ~GamemodeInstance() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	protected:
	Level& level;
	asset_ptr& assetManager;
};

class GameplayMode : public GamemodeInstance
{
	public:
	GameplayMode(Level& lvl, asset_ptr& am)
		: GamemodeInstance(lvl, am), player(this->level, {this->assetManager->playerSprites}),
		  inputHandler(this->player)
	{
		player.Reset(level.GetPlayerStartPos());
	};

	void Update() override;
	void Draw() override;
	void Reset();

	private:
	float gravity{1.5f};
	Player player;
	PlayerInputHandler inputHandler;
};

class EditMode : public GamemodeInstance
{
	public:
	EditMode(Level& lvl, asset_ptr& am) : GamemodeInstance(lvl, am) {};

	void Update() override;
	void Draw() override;

	private:
};
