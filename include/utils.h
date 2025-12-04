#pragma once

#include <cstdint>
#include <iostream>
#include <raylib.h>
#include <string>

static constexpr Color ERROR{220, 0, 0, 0};
static constexpr Color WARNING{230, 150, 0, 0};
static constexpr Color SUCCESS{0, 220, 0, 0};
static constexpr Color INFO{200, 200, 100, 0};

using byte = uint8_t;

struct Vector2Int
{
	int x{0};
	int y{0};

	bool operator==(const Vector2Int other)
	{
		return (this->x == other.x) && (this->y == other.y);
	}
	operator Vector2() const
	{
		return {static_cast<float>(this->x), static_cast<float>(this->y)};
	}
};

/** Resets all terminal styles and colours. */
inline void ClearStyles() { std::cout << "\033[0m"; }

/**
 * Sets the foreground colour of the output terminal to the provided colour
 * using ANSI escape codes.
 */
inline void SetTextColor(Color col)
{
	std::cout
		<< "\033[38;2;"
		<< std::to_string(col.r)
		<< ';'
		<< std::to_string(col.g)
		<< ';'
		<< std::to_string(col.b)
		<< 'm';
}
