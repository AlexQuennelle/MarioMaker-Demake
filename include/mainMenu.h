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

/** @brief Nullable pair of a @link SwitchRequest @endlink and a path to a .lvl
 *         file to load when switching modes.
 *  @note Uses @link std::optional @endlink to be nullable
 */
using ButtonResult = std::optional<std::pair<SwitchRequest, std::string>>;
/** @brief Alias for a @link std::function @endlink that takes in no parameters
 *         and returns a @link ButtonResult @endlink
 */
using ButtonEvent = std::function<ButtonResult()>;

/** @brief Abstract base button class for interactive UI.
 */
class ButtonBase // NOLINT
{
	public:
	ButtonBase(const Vector2Int pos, const Rectangle rect) :
		clickableArea(rect), position(pos) { };
	virtual ~ButtonBase() = default;

	/** @brief This method performs non-drawing related logic and should be
	 *         called every frame. If the provided point falls inside the
	 *         @link ButtonBase::clickableArea @endlink,
	 *         @link ButtonBase::hovered @endlink is set to true
	 *
	 *  @param mousePos Mouse position as a @link Vector2 @endlink
	 */
	virtual void Update(const Vector2 mousePos) = 0;
	/** @brief All drawing logic happens in this method
	 */
	virtual void Draw() = 0;
	/** @brief This method should be called when the mouse has been clicked or
	 *         equivalent input.
	 *
	 *  @returns An @link std::optional @endlink of a
	 *           @link ButtonEvent @endlink. This should be handled and called
	 *           by the caller to trigger the click function of the button.
	 */
	virtual auto OnClick() -> std::optional<ButtonEvent> = 0;

	protected:
	Rectangle clickableArea{};
	Vector2Int position{};
	bool hovered{false};
};

class MenuButton : public ButtonBase // NOLINT
{
	public:
	MenuButton(std::string text, const Vector2Int position,
			   const Rectangle rect, ButtonEvent eventFunc,
			   const int fontSize = 10);
	MenuButton() = delete;
	~MenuButton() override = default;

	/** @copydoc ButtonBase::Update()
	 */
	void Update(const Vector2 mousePos) override;
	/** @copydoc ButtonBase::Draw()
	 */
	void Draw() override;
	/** @copydoc ButtonBase::OnClick()
	 */
	auto OnClick() -> std::optional<ButtonEvent> override;

	private:
	const int fontSize;
	const std::string text;
	const ButtonEvent onClickEvent;
};

class LevelWidget : public ButtonBase // NOLINT
{
	using LoadFunc
		= std::function<ButtonResult(const SwitchRequest, const std::string&)>;

	public:
	LevelWidget(const std::string& filePath, const Vector2Int pos,
				const Rectangle rect, const LoadFunc& func);
	LevelWidget() = delete;
	~LevelWidget() override = default;

	auto operator==(const LevelWidget& other) -> bool
	{
		return (this->filePath == other.filePath);
	}
	auto operator==(const std::string& other) -> bool
	{
		return (this->filePath == other);
	}
	auto operator==(const char* other) -> bool
	{
		return (this->filePath == other);
	}

	/** @copydoc ButtonBase::Update()
	 *  @note Also calls Update() on @link LevelWidget::playButton @endlink and
	 *        @link LevelWidget::editButton @endlink
	 */
	void Update(const Vector2 mousePos) override;
	/** @copydoc ButtonBase::Draw()
	 */
	void Draw() override;
	/** @brief @copybrief ButtonBase::OnClick()
	 *  @note Also calls OnClick() on @link LevelWidget::playButton @endlink and
	 *        @link LevelWidget::editButton @endlink if
	 *        @link LevelWidget::hovered @endlink is true
	 *
	 *  @returns An @link std::optional @endlink of a
	 *           @link ButtonEvent @endlink. The contents of this optional are
	 *           provided by either @link LevelWidget::playButton @endlink or
	 *           @link LevelWidget::editButton @endlink if they are hovered.
	 *           This should be handled and called by the caller to trigger the
	 *           click function of the button.
	 */
	auto OnClick() -> std::optional<ButtonEvent> override;

	private:
	/** @brief Parses the header of the file pointed to by
	 *         @link LevelWidget::filePath @endlink to fetch some meta data.
	 */
	auto ParseHeader() -> bool;

	const std::string filePath;
	std::string levelName;
	std::unique_ptr<MenuButton> playButton;
	std::unique_ptr<MenuButton> editButton;
	bool isValid{false};
};
