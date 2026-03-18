#include "mainMenu.h"
#include "constants.h"
#include "gamemode.h"
#include "level.h"
#include "nfd.hpp"
#include "utils.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <print>
#include <raylib.h>
#include <rlgl.h>
#include <string>
#include <utility>

MainMenu::MainMenu(AssetManager& am, std::unique_ptr<Level>& lvl) :
	GamemodeInstance(nullptr, am), lvlPointer(lvl)
{
	this->camera = Camera2D{{0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};

	this->SwitchScreens(MenuScreen::Title);
}
void MainMenu::Update()
{
	float scaling{SCREEN_WIDTH / 384.0f};
	Vector2 mousePos{GetMousePosition() / scaling};

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
	if (this->currentScreen == MenuScreen::LevelSelect)
	{
		this->camera.offset.y
			= std::clamp(this->camera.offset.y + (GetMouseWheelMove() * 5.0f),
						 this->maxScrollOffset, 0.0f);
	}
}
void MainMenu::Draw()
{
	if (this->currentScreen == MenuScreen::LevelSelect)
	{
		ClearBackground({130, 140, 160, 255});
		this->DrawLevelList();
	}

	for (auto& button : this->buttons)
	{
		button->Draw();
	}
}
void MainMenu::DrawUI() { }

void MainMenu::InitTitleScreen()
{
	ButtonEvent switchToLevel = [this] -> ButtonResult
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
	float widgetHeight{30};

	ButtonEvent switchToTitle = [this] -> ButtonResult
	{
		return this->SwitchScreens(MenuScreen::Title);
	};

	this->buttons.push_back(std::make_unique<MenuButton>(
		"Back", Vector2Int{.x = 40, .y = 20},
		Rectangle{.x = -20.0f, .y = -7.0f, .width = 40.0f, .height = 14.0f},
		switchToTitle));

	auto loadFunc = [this](const SwitchRequest mode,
						   const std::string& filePath) -> ButtonResult
	{
		this->lvlPointer = std::make_unique<Level>(filePath, this->assetManager,
												   GameplayMode::gravity);
		this->switchReq = mode;
		return std::nullopt;
	};
#if !defined(PLATFORM_WEB)
	auto altLoad = [this, widgetHeight, loadFunc] -> ButtonResult
	{
		NFD::Guard nfdGuard;
		NFD::UniquePath outPath;
		nfdfilteritem_t filter{"Level Files", "lvl"};
		nfdresult_t result{
			NFD::OpenDialog(outPath, &filter, 1, RESOURCES_PATH)};
		if (result == NFD_OKAY)
		{
			auto pred = [&outPath](auto& widget) -> auto
			{
				return static_cast<LevelWidget&>(*widget) == outPath.get();
			};
			if (std::ranges::any_of(this->levels, pred))
				return std::nullopt;

			float levelsHeight{
				(widgetHeight * static_cast<float>(this->levels.size()))
					+ static_cast<float>(this->levels.size()),
			};
			this->levels.push_back(std::make_unique<LevelWidget>(
				std::string(outPath.get()),
				Vector2Int{.x = 192, .y = static_cast<int>(30 + levelsHeight)},
				Rectangle{-75.0f, 0.0f, 150.0f, widgetHeight}, loadFunc));
			levelsHeight += 30.0f;
			this->maxScrollOffset = -std::max(levelsHeight - 156.0f, 0.0f);
			return std::nullopt;
		}
		else if (result == NFD_ERROR)
		{
			std::cerr << NFD_GetError() << '\n';
			return std::nullopt;
		}
		else if (result == NFD_CANCEL)
		{
			std::cout << "Load cancelled.\n";
			return std::nullopt;
		}
		return std::nullopt;
	};
	this->buttons.push_back(std::make_unique<MenuButton>(
		"Load Level", Vector2Int(384 - 80, 20),
		Rectangle{.x = -30.0f, .y = -7.0f, .width = 60.0f, .height = 14.0f},
		altLoad));
#endif
	auto newLevel = [this] -> ButtonResult
	{
		this->lvlPointer = std::make_unique<Level>(this->assetManager, "");
		this->switchReq = SwitchRequest::EditMode;
		return std::nullopt;
	};
	this->buttons.push_back(std::make_unique<MenuButton>(
		"New Level", Vector2Int(384 - 80, 36),
		Rectangle{.x = -30.0f, .y = -7.0f, .width = 60.0f, .height = 14.0f},
		newLevel));

	int offset{0};
	namespace fs = std::filesystem;
	for (const auto& entry : fs::directory_iterator(RESOURCES_PATH))
	{
		std::string path = entry.path().string();
		if (path.substr(path.length() - 4, 4) == ".lvl")
		{
			this->levels.push_back(std::make_unique<LevelWidget>(
				path, Vector2Int{.x = 192, .y = 30 + offset},
				Rectangle{-75.0f, 0.0f, 150.0f, widgetHeight}, loadFunc));
			offset += static_cast<int32_t>(widgetHeight + 1);
		}
	}

	float levelsHeight{
		(widgetHeight * static_cast<float>(this->levels.size()))
			+ static_cast<float>(this->levels.size() - 1),
	};
	this->maxScrollOffset = -std::max(levelsHeight - 156.0f, 0.0f);
}

void MainMenu::DrawLevelList()
{
	DrawRectangleRec({114.0f, 0.0f, 156.0f,
					  60.0f
						  + (30.0f
							 * static_cast<float>(this->levels.size())
							 + static_cast<float>(this->levels.size())
							 - 1.0f)},
					 {100, 110, 140, 255});
	DrawRectangleRec({114.0f, 0.0f, 156.0f, 216.0f}, {100, 110, 140, 255});
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
	this->lvlPointer
		= std::make_unique<Level>(result->second, this->assetManager);
	this->switchReq = result->first;
}

auto MainMenu::SwitchScreens(MenuScreen screen) -> ButtonResult
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
					   const int fontSize) :
	ButtonBase(pos, rect),
	fontSize(fontSize),
	text(std::move(text)),
	onClickEvent(std::move(eventFunc))
{ }
void MenuButton::Update(const Vector2 mousePos)
{
	Vector2 relativeMousePos = mousePos - this->position;
	this->hovered
		= CheckCollisionPointRec(relativeMousePos, this->clickableArea);
}
void MenuButton::Draw()
{
	Vector2 center{
		static_cast<float>(this->position.x)
			+ this->clickableArea.x
			+ (this->clickableArea.width / 2.0f),
		static_cast<float>(this->position.y)
			+ this->clickableArea.y
			+ (this->clickableArea.height / 2.0f),
	};
	Vector2 topLeft{
		static_cast<float>(this->position.x) + this->clickableArea.x,
		static_cast<float>(this->position.y) + this->clickableArea.y,
	};
	Vector2 textSize{MeasureTextEx(GetFontDefault(), this->text.c_str(),
								   static_cast<float>(this->fontSize), 1.0f)};

	if (this->hovered)
	{
		DrawRectangle(static_cast<int32_t>(topLeft.x),
					  static_cast<int32_t>(topLeft.y),
					  static_cast<int32_t>(this->clickableArea.width),
					  static_cast<int32_t>(this->clickableArea.height), WHITE);
	}
	else
	{
		DrawRectangle(static_cast<int32_t>(topLeft.x),
					  static_cast<int32_t>(topLeft.y),
					  static_cast<int32_t>(this->clickableArea.width),
					  static_cast<int32_t>(this->clickableArea.height), GRAY);
	}

	DrawTextEx(GetFontDefault(), this->text.c_str(), center - (textSize / 2.0f),
			   static_cast<float>(this->fontSize), 1.0f, BLACK);
}
auto MenuButton::OnClick() -> std::optional<ButtonEvent>
{
	if (this->hovered)
		return onClickEvent;
	else
		return {};
}

LevelWidget::LevelWidget(const std::string& filePath, const Vector2Int pos,
						 const Rectangle rect, const LoadFunc& func) :
	ButtonBase(pos, rect), filePath(filePath), isValid(this->ParseHeader())
{
	auto play = [func, filePath] -> ButtonResult
	{
		return func(SwitchRequest::GameplayMode, filePath);
	};
	auto edit = [func, filePath] -> ButtonResult
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
		mousePos.x - static_cast<float>(this->position.x),
		mousePos.y - static_cast<float>(this->position.y),
	};
	this->editButton->Update(adjustedPos);
	this->playButton->Update(adjustedPos);
};
void LevelWidget::Draw()
{
	rlPushMatrix();
	rlTranslatef(static_cast<float>(this->position.x),
				 static_cast<float>(this->position.y), 0.0f);

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
auto LevelWidget::OnClick() -> std::optional<ButtonEvent>
{
	if (!this->isValid)
		return std::nullopt;

	std::optional<ButtonEvent> playFunc{this->editButton->OnClick()};
	std::optional<ButtonEvent> editFunc{this->playButton->OnClick()};

	if (playFunc.has_value())
		return playFunc;
	else if (editFunc.has_value())
		return editFunc;
	else
		return std::nullopt;
}
auto LevelWidget::ParseHeader() -> bool
{
	namespace fs = std::filesystem;
	using std::ios;
	std::ifstream file{this->filePath.c_str(), ios::binary | ios::ate};
	if (file.is_open())
	{
		auto fSize = fs::file_size(this->filePath);
		std::vector<char> data(fSize, 0);

		file.seekg(0, ios::beg);
		file.read(data.data(), static_cast<int64_t>(fSize));
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
			std::cerr
				<< "ERROR: "
				<< this->filePath
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
