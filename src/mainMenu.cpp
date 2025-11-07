#include "mainMenu.h"
#include "gamemode.h"

#include <functional>
#include <memory>
#include <optional>
#include <raylib.h>
#include <utility>

MainMenu::MainMenu(AssetManager& am, std::unique_ptr<Level>& lvl)
	: GamemodeInstance(nullptr, am), lvlPointer(lvl)
{
	this->camera = Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};

	// Init base lambdas
	this->SwitchScreens = [this](MenuScreen screen)
	{
		this->currentScreen = screen;
		return std::nullopt;
	};

	this->InitTitleScreen();
	this->InitLevelScreen();
}
void MainMenu::Update()
{
	if (IsKeyPressed(KEY_P))
	{
		this->switchReq = SwitchRequest::GameplayMode;
	}
	switch (this->currentScreen)
	{
		using enum MenuScreen;
	case (Title):
		UpdateTitleScreen();
		break;
	case (LevelSelect):
		UpdateLevelScreen();
		break;
	default:
		break;
	}
}
void MainMenu::Draw()
{
	switch (this->currentScreen)
	{
		using enum MenuScreen;
	case (Title):
		this->DrawTitleScreen();
		break;
	case (LevelSelect):
		this->DrawLevelScreen();
		break;
	default:
		break;
	}
}
void MainMenu::DrawUI() {}

void MainMenu::InitTitleScreen()
{
	ButtonEvent switchToLevel = [this]
	{
		return this->SwitchScreens(MenuScreen::LevelSelect);
	};

	this->titleScreenButtons.push_back(std::make_unique<MenuButton>(
		Vector2Int{.x = 192, .y = 108},
		Rectangle{.x = -70.0f, .y = -20.0f, .width = 140.0f, .height = 40.0f},
		switchToLevel));
}
void MainMenu::UpdateTitleScreen()
{
	Vector2 mousePos{GetMousePosition()};
	for (auto& button : this->titleScreenButtons)
	{
		button->Update(mousePos);
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (auto& button : this->titleScreenButtons)
		{
			std::optional<ButtonEvent> func{button->OnClick()};
			if (func.has_value())
			{
				ButtonResult result{func.value()()};
				if (result.has_value())
					HandleButtonResult(result);
				break;
			}
		}
	}
}
void MainMenu::DrawTitleScreen()
{
	for (auto& button : this->titleScreenButtons)
	{
		button->Draw();
	}
}

void MainMenu::InitLevelScreen()
{
	ButtonEvent switchToTitle = [this]
	{
		return this->SwitchScreens(MenuScreen::Title);
	};

	this->levelScreenButtons.push_back(std::make_unique<MenuButton>(
		Vector2Int{.x = 192, .y = 108},
		Rectangle{.x = -20.0f, .y = -7.0f, .width = 40.0f, .height = 14.0f},
		switchToTitle));
}
void MainMenu::UpdateLevelScreen()
{
	Vector2 mousePos{GetMousePosition()};
	for (auto& button : this->levelScreenButtons)
	{
		button->Update(mousePos);
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (auto& button : this->levelScreenButtons)
		{
			std::optional<ButtonEvent> func{button->OnClick()};
			if (func.has_value())
			{
				ButtonResult result{func.value()()};
				if (result.has_value())
					HandleButtonResult(result);
				break;
			}
		}
	}
}
void MainMenu::DrawLevelScreen()
{
	for (auto& button : this->levelScreenButtons)
	{
		button->Draw();
	}
}

void MainMenu::HandleButtonResult(ButtonResult result)
{
	this->lvlPointer =
		std::make_unique<Level>(result->second, &this->assetManager);
	this->switchReq = result->first;
}

//UI functionality

MenuButton::MenuButton(const Vector2Int pos, const Rectangle rect,
					   ButtonEvent eventFunc)
	: position(pos), clickableArea(rect), onClickEvent(std::move(eventFunc))
{}
void MenuButton::Update(const Vector2 mousePos)
{
	float scaling{GetScreenWidth() / 384.0f};
	Vector2 relativeMousePos = mousePos / scaling - this->position;
	this->hovered =
		CheckCollisionPointRec(relativeMousePos, this->clickableArea);
	// if (hovered)
	// 	std::cout << std::format("{:.1f}, {:.1f}\n", relativeMousePos.x,
	// 							 relativeMousePos.y);
}
void MenuButton::Draw()
{
	if (this->hovered)
	{
		DrawRectangle(this->position.x + this->clickableArea.x,
					  this->position.y + this->clickableArea.y,
					  this->clickableArea.width, this->clickableArea.height,
					  WHITE);
	}
	else
	{
		DrawRectangle(this->position.x + this->clickableArea.x,
					  this->position.y + this->clickableArea.y,
					  this->clickableArea.width, this->clickableArea.height,
					  GRAY);
	}
}
std::optional<ButtonEvent> MenuButton::OnClick()
{
	if (this->hovered)
		return onClickEvent;
	else
		return {};
}
