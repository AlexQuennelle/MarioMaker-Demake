#include "mainMenu.h"
#include "gamemode.h"
#include "utils.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <raylib.h>
#include <rlgl.h>
#include <string>
#include <utility>

MainMenu::MainMenu(AssetManager& am, std::unique_ptr<Level>& lvl)
	: GamemodeInstance(nullptr, am), lvlPointer(lvl)
{
	this->camera = Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};

	// this->SwitchScreens(MenuScreen::Title);
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

	for (auto& button : this->buttons)
	{
		button->Update(mousePos);
	}
	for (auto& widget : this->levels)
	{
		widget->Update(mousePos);
	}
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (auto& button : this->buttons)
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
		for (auto& widget : this->levels)
		{
			std::optional<ButtonEvent> func{widget->OnClick()};
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
	if (this->currentScreen == MenuScreen::LevelSelect)
		ClearBackground({130, 140, 160, 255});

	for (auto& button : this->buttons)
	{
		button->Draw();
	}
	this->DrawLevelList();
}
void MainMenu::DrawUI() {}

void MainMenu::InitTitleScreen()
{
	ButtonEvent switchToLevel = [this]
	{
		return this->SwitchScreens(MenuScreen::LevelSelect);
	};

	this->buttons.push_back(std::make_unique<MenuButton>(
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

	this->buttons.push_back(std::make_unique<MenuButton>(
		"Back", Vector2Int{.x = 40, .y = 20},
		Rectangle{.x = -20.0f, .y = -7.0f, .width = 40.0f, .height = 14.0f},
		switchToTitle));

	auto loadFunc =
		[this](const SwitchRequest mode, const std::string& filePath)
	{
		//return this->LoadLevel(mode, filePath);
		this->lvlPointer =
			std::make_unique<Level>(filePath, this->assetManager);
		this->switchReq = mode;
		return std::nullopt;
	};
	int offset{0};
	float widgetHeight{30};
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator(RESOURCES_PATH))
	{
		std::string path = entry.path().string();
		if (path.substr(path.length() - 4, 4) == ".lvl")
		{
			this->levels.push_back(std::make_unique<LevelWidget>(
				path, Vector2Int{.x = 192, .y = 30 + offset},
				//RESOURCES_PATH "1-1.lvl", Vector2Int{.x = 192, .y = 108},
				Rectangle{-75.0f, 0.0f, 150.0f, widgetHeight}, loadFunc));
			offset += widgetHeight + 1;
		}
	}
}

void MainMenu::DrawLevelList()
{
	DrawRectangleRec({114.0f, 27.0f, 156.0f, 162.0f}, {100, 110, 140, 255});
	BeginMode2D(this->camera);
	for (auto& widget : this->levels)
	{
		widget->Draw();
	}
	EndMode2D();
	DrawRectangleRec({114.0f, 0.0f, 156.0f, 27.0f}, {130, 140, 160, 255});
	DrawRectangleRec({114.0f, 189.0f, 156.0f, 27.0f}, {130, 140, 160, 255});
}

void MainMenu::HandleButtonResult(ButtonResult result)
{
	this->lvlPointer =
		std::make_unique<Level>(result->second, this->assetManager);
	this->switchReq = result->first;
}

ButtonResult MainMenu::SwitchScreens(MenuScreen screen)
{
	this->buttons.clear();
	this->levels.clear();
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
	: ButtonBase(pos, rect), text(std::move(text)),
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
								   this->fontSize, 1.0f)};

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

	DrawTextEx(GetFontDefault(), this->text.c_str(), center - (textSize / 2.0f),
			   this->fontSize, 1.0f, BLACK);
}
std::optional<ButtonEvent> MenuButton::OnClick()
{
	if (this->hovered)
		return onClickEvent;
	else
		return {};
}

LevelWidget::LevelWidget(const std::string& filePath, const Vector2Int pos,
						 const Rectangle rect, const LoadFunc& func)
	: ButtonBase(pos, rect), filePath(filePath)
{
	this->isValid = this->ParseHeader();

	auto play = [func, filePath]
	{
		return func(SwitchRequest::GameplayMode, filePath);
	};
	auto edit = [func, filePath]
	{
		return func(SwitchRequest::EditMode, filePath);
	};
	this->playButton = std::make_unique<MenuButton>(
		"Play", Vector2Int{.x = 61, .y = 2},
		Rectangle{-12.0f, 0.0f, 24.0f, 12.0f}, play, 10);
	this->editButton = std::make_unique<MenuButton>(
		"Edit", Vector2Int{.x = 61, .y = 16},
		Rectangle{-12.0f, 0.0f, 24.0f, 12.0f}, edit, 10);
}
void LevelWidget::Update(const Vector2 mousePos)
{
	Vector2 adjustedPos{
		mousePos.x - this->position.x,
		mousePos.y - this->position.y,
	};
	this->editButton->Update(adjustedPos);
	this->playButton->Update(adjustedPos);
};
void LevelWidget::Draw()
{
	rlPushMatrix();
	rlTranslatef(this->position.x, this->position.y, 0.0f);

	if (this->isValid)
	{
		DrawRectangleRec(this->clickableArea, {100, 100, 110, 255});
		DrawTextEx(GetFontDefault(), this->levelName.c_str(),
				   {this->clickableArea.x + 2.0f, this->clickableArea.y + 2.0f},
				   10.0f, 1.0f, WHITE);

		this->editButton->Draw();
		this->playButton->Draw();
	}
	else
	{
		DrawRectangleRec(this->clickableArea, {160, 60, 50, 255});
	}

	rlPopMatrix();
}
std::optional<ButtonEvent> LevelWidget::OnClick()
{
	if (!this->isValid)
		return std::nullopt;

	std::optional<ButtonEvent> playFunc{this->editButton->OnClick()};
	std::optional<ButtonEvent> editFunc{this->playButton->OnClick()};

	if (playFunc.has_value())
		return playFunc.value();
	else if (editFunc.has_value())
		return editFunc.value();
	else
		return std::nullopt;
}
bool LevelWidget::ParseHeader()
{
	namespace fs = std::filesystem;
	using std::ios;
	std::ifstream file{this->filePath.c_str(), ios::binary | ios::ate};
	if (file.is_open())
	{
		std::streampos fSize = fs::file_size(this->filePath);
		std::vector<char> data(fSize, 0);

		file.seekg(0, ios::beg);
		file.read(data.data(), fSize);
		file.close();

		std::string fileID(3, 0);
		std::memcpy(fileID.data(), data.data(), 3);

		if (fileID == "LVL")
		{
			uint32_t nameLen{0};
			std::memcpy(&nameLen, &data[20], 4);
			this->levelName = std::string(nameLen, 0);
			std::memcpy(this->levelName.data(), &data[24], nameLen);
		}
		else
		{
			SetTextColor(ERROR);
			std::cerr << "ERROR: " << this->filePath
					  << " is not a valid level file\n";
			ClearStyles();
			return false;
		}
	}
	else
	{
		SetTextColor(ERROR);
		std::cerr << "ERROR: Unable to open file at " << this->filePath << '\n';
		ClearStyles();
		return false;
	}

	return true;
}
