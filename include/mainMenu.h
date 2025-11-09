#pragma once

#include "utils.h"

#include <cstdint>
#include <functional>
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

class MenuButton
{
	public:
	MenuButton(std::string text, const Vector2Int position,
			   const Rectangle rect, ButtonEvent eventFunc,
			   const int fontSize = 10);
	MenuButton() = delete;

	void Update(const Vector2 mousePos);
	void Draw();
	std::optional<ButtonEvent> OnClick();

	private:
	Vector2Int position;
	Rectangle clickableArea;
	bool hovered{false};
	const int fontSize;
	const std::string text;
	const ButtonEvent onClickEvent;
};

class LevelWidget
{
	public:
	private:
};
