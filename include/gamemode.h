#pragma once

#include "assetmanager.h"
#include "imgui.h"
#include "level.h"
#include "player.h"
#include "playerInputHandler.h"
#include "tile.h"
#include "gameUIDisplay.h"

#include <array>
#include <raylib.h>
#include <string>

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
	float time{0};
	Player player;
	PlayerInputHandler inputHandler;
	GameUIDisplay uiDisplay;
};

class EditMode : public GamemodeInstance
{
	public:
	EditMode(Level& lvl, asset_ptr& am, const ImGuiIO& imgui);

	void Update() override;
	void Draw() override;
	void DrawUI() override;

	private:
	void DrawButtons();
	void DrawPallette();
	void SaveLevel();
	void SaveLevelAs();

	RenderTexture tex;
	Vector2Int selectedTile;
	Vector2 lvlMousePos;
	const ImGuiIO& imGuiIO;
	Tile brush{.ID = TileID::ground, .flags = 0};
	const std::array<std::string, 4> tileNames{
		"Ground",
		"Bricks",
		"Spikes",
		"Item Box",
	};
};
