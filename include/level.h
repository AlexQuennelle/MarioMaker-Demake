#pragma once

#include "tile.h"

#include <cstdint>
#include <vector>

using std::vector;

class Level
{
	public:
	Level();

	void Draw();
	void Reset();

	void SetTileAt(const TileID tile, const int x, const int y);
	TileID TileAt(const int x, const int y);

	private:
	int32_t height;
	int32_t length;
	vector<TileID> grid;
	// TODO: Vector of entity
	// TODO: Collision map
};
