#pragma once

#include "assetmanager.h"
#include "gameUIDisplay.h"
#include "level.h"
#include "mainMenu.h"
#include "player.h"
#include "playerInputHandler.h"
#include "tile.h"
#include "utils.h"

#include <array>
#include <cstdint>
#include <imgui.h>
#include <memory>
#include <raylib.h>
#include <string>

enum class SwitchRequest : uint8_t
{
	None,
	MainMenu,
	GameplayMode,
	EditMode,
};

class GamemodeInstance
{
	public:
	GamemodeInstance(Level* lvl, AssetManager& am)
		: level(lvl), assetManager(am) {};
	virtual ~GamemodeInstance() = default;

	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void DrawUI() = 0;

	SwitchRequest GetNextMode() { return this->switchReq; }

	Camera2D camera{0};

	protected:
	SwitchRequest switchReq{SwitchRequest::None};
	Level* level;
	AssetManager& assetManager;
};

class GameplayMode : public GamemodeInstance
{
	public:
	GameplayMode(Level* lvl, AssetManager& am);
	~GameplayMode() override = default;

	void Update() override;
	void Draw() override;
	void DrawUI() override;
	void Reset();

	static constexpr float gravity{0.9f};

	private:
	float timeDead{0};
	float time{0};
	Player player;
	PlayerInputHandler inputHandler;
	GameUIDisplay uiDisplay;
};

class EditMode : public GamemodeInstance
{
	public:
	EditMode(Level* lvl, AssetManager& am, const ImGuiIO& imgui);
	~EditMode() override;

	void Update() override;
	void Draw() override;
	void DrawUI() override;

	private:
	void DrawButtons();
	void DrawPallette();

	void ProcessInput();

	void SaveLevel();
#if !defined(PLATFORM_WEB)
	void SaveLevelAs();
#endif

	RenderTexture tex;
	Vector2Int selectedTile{.x = 0, .y = 0};
	Vector2 lvlMousePos;
	const ImGuiIO& imGuiIO;
	Tile brush{.ID = TileID::ground, .flags = 0};
	const std::array<std::string, 8> tileNames{
		"Ground", "Bricks",		   "Spikes",	   "Item Box",
		"Coin",	  "Toggle Switch", "Toggle Block", "Mushroom",
	};
};

class MainMenu : public GamemodeInstance
{
	public:
	MainMenu(AssetManager& am, std::unique_ptr<Level>& lvl);
	~MainMenu() override = default;

	void Update() override;
	void Draw() override;
	void DrawUI() override;

	private:
	void InitTitleScreen();
	void InitLevelScreen();

	void DrawLevelList();

	void HandleButtonResult(ButtonResult result);

	ButtonResult SwitchScreens(MenuScreen screen);

	// Member variables
	float maxScrollOffset{0.0f};
	MenuScreen currentScreen{MenuScreen::Title};
	std::unique_ptr<Level>& lvlPointer;
	vector<std::unique_ptr<ButtonBase>> buttons;
	vector<std::unique_ptr<ButtonBase>> levels;
};
