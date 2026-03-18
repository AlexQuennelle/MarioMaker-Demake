#pragma once

#include <cstdint>

enum class TileID : uint8_t
{
	air,
	ground,
	brick,
	spikes,
	itemBox,
	coin,
	toggleSwitch,
	toggleBlock,
	mushroom,
	walkerEnemy,
	fireFlower,
	podoboo,
};

struct Tile
{
	TileID ID;
	uint16_t flags;

	auto operator==(const Tile& other) -> bool
	{
		return (this->ID == other.ID) && (this->flags == other.flags);
	}
};
