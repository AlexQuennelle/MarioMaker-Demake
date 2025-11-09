#include "mainMenu.h"
#include "gamemode.h"

#include <functional>
#include <memory>
#include <optional>
#include <raylib.h>
#include <string>
#include <utility>

MainMenu::MainMenu(AssetManager& am, std::unique_ptr<Level>& lvl)
	: GamemodeInstance(nullptr, am), lvlPointer(lvl)
{
	this->camera = Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};

	//this->SwitchScreens(MenuScreen::Title);
	this->SwitchScreens(MenuScreen::LevelSelect);
}
void MainMenu::Update()
{
	float scaling{GetScreenWidth() / 384.0f};
	Vector2 mousePos{GetMousePosition() / scaling};

	if (IsKeyPressed(KEY_P))
	{
		this->switchReq = SwitchRequest::GameplayMode;
	}

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
void MainMenu::Draw()
{
	for (auto& button : this->titleScreenButtons)
	{
		button->Draw();
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
		"Play", Vector2Int{.x = 192, .y = 108},
		Rectangle{.x = -70.0f, .y = -20.0f, .width = 140.0f, .height = 40.0f},
		switchToLevel, 20));
}
void MainMenu::InitLevelScreen()
{
	ButtonEvent switchToTitle = [this]
	{
		return this->SwitchScreens(MenuScreen::Title);
	};

	this->titleScreenButtons.push_back(std::make_unique<MenuButton>(
		"Back", Vector2Int{.x = 40, .y = 20},
		Rectangle{.x = -20.0f, .y = -7.0f, .width = 40.0f, .height = 14.0f},
		switchToTitle));
}

void MainMenu::DrawLevelList()
{
}

void MainMenu::HandleButtonResult(ButtonResult result)
{
	this->lvlPointer =
		std::make_unique<Level>(result->second, &this->assetManager);
	this->switchReq = result->first;
}

ButtonResult MainMenu::SwitchScreens(MenuScreen screen)
{
	this->titleScreenButtons.clear();
	this->currentScreen = screen;
	this->camera.offset = {.x = 0.0f, .y = 0.0f};

	switch (screen)
	{
		using enum MenuScreen;
	case (Title):
		this->InitTitleScreen();
		break;
	case (LevelSelect):
		this->InitLevelScreen();
		break;
	default:
		break;
	}

	return std::nullopt;
}

//UI functionality

MenuButton::MenuButton(std::string text, const Vector2Int pos,
					   const Rectangle rect, ButtonEvent eventFunc,
					   const int fontSize)
	: position(pos), clickableArea(rect), text(std::move(text)),
	  onClickEvent(std::move(eventFunc)), fontSize(fontSize)
{}
void MenuButton::Update(const Vector2 mousePos)
{
	Vector2 relativeMousePos = mousePos - this->position;
	this->hovered =
		CheckCollisionPointRec(relativeMousePos, this->clickableArea);
}
void MenuButton::Draw()
{
	Vector2 center{
		this->position.x + this->clickableArea.x +
			(this->clickableArea.width / 2.0f),
		this->position.y + this->clickableArea.y +
			(this->clickableArea.height / 2.0f),
	};
	Vector2 topLeft{
		this->position.x + this->clickableArea.x,
		this->position.y + this->clickableArea.y,
	};
	Vector2 textSize{MeasureTextEx(GetFontDefault(), this->text.c_str(),
								   this->fontSize, 0.0f)};

	if (this->hovered)
	{
		DrawRectangle(topLeft.x, topLeft.y, this->clickableArea.width,
					  this->clickableArea.height, WHITE);
	}
	else
	{
		DrawRectangle(topLeft.x, topLeft.y, this->clickableArea.width,
					  this->clickableArea.height, GRAY);
	}
	DrawText(this->text.c_str(), center.x - (textSize.x / 2.0f),
			 center.y - (textSize.y / 2.0f), this->fontSize, BLACK);
}
std::optional<ButtonEvent> MenuButton::OnClick()
{
	if (this->hovered)
		return onClickEvent;
	else
		return {};
}
