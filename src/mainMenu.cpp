#include "gamemode.h"

#include <raylib.h>

MainMenu::MainMenu(Level* lvl, AssetManager& am) : GamemodeInstance(lvl, am)
{
	this->camera = Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};
}
void MainMenu::Update() {}
void MainMenu::Draw() {}
void MainMenu::DrawUI() {}
