#pragma once

#include "assetmanager.h"
#include "level.h"
#include "player.h"
#include "playerInputHandler.h"

#include <raylib.h>

class GamemodeInstance
{
	public:
	GamemodeInstance(Level& lvl, asset_ptr& am)
		: level(lvl), assetManager(am) {};
	virtual ~GamemodeInstance() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawUI() = 0;

	Camera2D camera{0};

	protected:
	Level& level;
	asset_ptr& assetManager;
};

class GameplayMode : public GamemodeInstance
{
	public:
	GameplayMode(Level& lvl, asset_ptr& am);

	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void Reset();

	private:
	float timeDead{0};
	float gravity{0.9f};
	Player player;
	PlayerInputHandler inputHandler;
};

class EditMode : public GamemodeInstance
{
	public:
	EditMode(Level& lvl, asset_ptr& am);

	void Update() override;
	void Draw() override;
	void DrawUI() override;

	private:
	RenderTexture tex;
	Vector2Int selectedTile;
	Vector2 lvlMousePos;
};
