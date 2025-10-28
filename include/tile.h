#pragma once

#include <cstdint>

enum class TileID : uint8_t
{
	air,
	ground,
	spikes,
	itemBox,
};

struct Tile
{
	TileID ID;
	uint8_t flags;

	bool operator==(const Tile& other)
	{
		return (this->ID == other.ID) && (this->flags == other.flags);
	}
};
