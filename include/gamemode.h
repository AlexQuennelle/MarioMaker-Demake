#pragma once

#include "level.h"
#include "player.h"
#include "playerInputHandler.h"
#include <raylib.h>

class GamemodeInstance
{
	public:
	GamemodeInstance(Level& lvl) : level(lvl) {};
	virtual ~GamemodeInstance() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawUI() = 0;

	Camera2D camera{0};
	protected:
	Level& level;
};

class GameplayMode : public GamemodeInstance
{
	public:
	GameplayMode(Level& lvl);

	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void Reset();

	private:
	float gravity{1.5f};
	Player player;
	PlayerInputHandler inputHandler;
};

class EditMode : public GamemodeInstance
{
	public:
	EditMode(Level& lvl);

	void Update() override;
	void Draw() override;
	void DrawUI() override;

	private:
};
