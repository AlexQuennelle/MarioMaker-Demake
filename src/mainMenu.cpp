#include "mainMenu.h"
#include "gamemode.h"

#include <raylib.h>

MainMenu::MainMenu(AssetManager& am) : GamemodeInstance(nullptr, am)
{
	this->camera = Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};
}
void MainMenu::Update()
{
	if (IsKeyPressed(KEY_E))
	{
		this->switchReq = SwitchRequest::EditMode;
	}
	else if (IsKeyPressed(KEY_P))
	{
		this->switchReq = SwitchRequest::GameplayMode;
	}
}
void MainMenu::Draw() {}
void MainMenu::DrawUI() {}
