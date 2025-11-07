#include "mainMenu.h"
#include "gamemode.h"

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <raylib.h>
#include <string>
#include <utility>

MainMenu::MainMenu(AssetManager& am, std::unique_ptr<Level>& lvl)
	: GamemodeInstance(nullptr, am), lvlPointer(lvl)
{
	this->camera = Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};
	auto exampleFunc = [this]()
		{ // 
			std::pair<SwitchRequest, std::string> pair{
				SwitchRequest::EditMode, RESOURCES_PATH "1-1.lvl",
			};
			std::cout << "click!\n";
			return pair;
		};
	this->mainScreenButtons.push_back(std::make_unique<MenuButton>(
		Vector2Int{.x = 100, .y = 150}, Rectangle{-25.0f, -15.0f, 25.0f, 15.0f},
		exampleFunc));
}
void MainMenu::Update()
{
	Vector2 mousePos{GetMousePosition()};
	if (IsKeyPressed(KEY_E))
	{
		this->switchReq = SwitchRequest::EditMode;
	}
	else if (IsKeyPressed(KEY_P))
	{
		this->switchReq = SwitchRequest::GameplayMode;
	}
	for (auto& button : this->mainScreenButtons)
	{
		button->Update(mousePos);
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (auto& button : this->mainScreenButtons)
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
	for (auto& button : this->mainScreenButtons)
	{
		button->Draw();
	}
}
void MainMenu::DrawUI() {}

void MainMenu::HandleButtonResult(ButtonResult result)
{
	this->lvlPointer =
		std::make_unique<Level>(result->second, &this->assetManager);
	this->switchReq = result->first;
}

//UI functionality

MenuButton::MenuButton(const Vector2Int pos, const Rectangle rect,
					   ButtonEvent& eventFunc)
	: position(pos), clickableArea(rect), onClickEvent(eventFunc)
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
