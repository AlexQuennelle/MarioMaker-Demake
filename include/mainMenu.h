#pragma once

#include "utils.h"

#include <cstdint>
#include <functional>
#include <optional>
#include <raylib.h>
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
	MenuButton(const Vector2Int position, const Rectangle rect,
			   ButtonEvent eventFunc);
	MenuButton() = delete;

	void Update(const Vector2 mousePos);
	void Draw();
	std::optional<ButtonEvent> OnClick();

	private:
	Vector2Int position;
	Rectangle clickableArea;
	bool hovered{false};
	ButtonEvent onClickEvent;
};
