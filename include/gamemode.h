#pragma once

#include "level.h"
#include "player.h"
#include "playerInputHandler.h"

class GamemodeInstance
{
	public:
	GamemodeInstance(Level& lvl) : level(lvl) {};
	virtual ~GamemodeInstance() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;

	protected:
	Level& level;
};

class GameplayMode : public GamemodeInstance
{
	public:
	GameplayMode(Level& lvl)
		: GamemodeInstance(lvl), player(this->level), inputHandler(this->player)
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
	EditMode(Level& lvl) : GamemodeInstance(lvl) {};

	void Update() override;
	void Draw() override;

	private:
};
