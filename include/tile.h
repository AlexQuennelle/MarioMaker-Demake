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
};

struct Tile
{
	TileID ID;
	uint16_t flags;

	bool operator==(const Tile& other)
	{
		return (this->ID == other.ID) && (this->flags == other.flags);
	}
};
