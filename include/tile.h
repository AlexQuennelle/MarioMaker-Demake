#pragma once

#include <cstdint>

enum class TileID : uint8_t
{
	air,
	ground,
	spikes,
	itemBox,
};

/**
 * Interface class to denote an object as a tile that can be placed in the level
 */
class ITile
{
	public:
	private:
};

class GroundTile : public ITile
{
	public:
	private:
};
