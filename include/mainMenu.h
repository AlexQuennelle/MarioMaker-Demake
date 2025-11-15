#pragma once

#include "utils.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <raylib.h>
#include <string>
#include <utility>

enum class SwitchRequest : uint8_t;

enum class MenuScreen : uint8_t
{
	Title,
	LevelSelect,
};

using ButtonResult = std::optional<std::pair<SwitchRequest, std::string>>;
using ButtonEvent = std::function<ButtonResult()>;

class ButtonBase
{
	public:
	ButtonBase(const Vector2Int pos, const Rectangle rect)
		: position(pos), clickableArea(rect) {};
	virtual ~ButtonBase() = default;

	virtual void Update(const Vector2 mousePos) = 0;
	virtual void Draw() = 0;
	virtual std::optional<ButtonEvent> OnClick() = 0;

	protected:
	Rectangle clickableArea;
	Vector2Int position;
	bool hovered{false};
};

class MenuButton : public ButtonBase
{
	public:
	MenuButton(std::string text, const Vector2Int position,
			   const Rectangle rect, ButtonEvent eventFunc,
			   const int fontSize = 10);
	MenuButton() = delete;
	~MenuButton() override = default;

	void Update(const Vector2 mousePos) override;
	void Draw() override;
	std::optional<ButtonEvent> OnClick() override;

	private:
	const int fontSize;
	const std::string text;
	const ButtonEvent onClickEvent;
};

class LevelWidget : public ButtonBase
{
	using LoadFunc =
		std::function<ButtonResult(const SwitchRequest, const std::string&)>;

	public:
	LevelWidget(const std::string& filePath, const Vector2Int pos,
				const Rectangle rect, const LoadFunc& func);
	LevelWidget() = delete;
	~LevelWidget() override = default;

	void Update(const Vector2 mousePos) override;
	void Draw() override;
	std::optional<ButtonEvent> OnClick() override;

	private:
	bool ParseHeader();

	bool isValid{false};
	std::string levelName;
	const std::string filePath;
	std::unique_ptr<MenuButton> playButton;
	std::unique_ptr<MenuButton> editButton;
};
