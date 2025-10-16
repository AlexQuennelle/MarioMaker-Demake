#pragma once

#include "tile.h"

#include <cstdint>
#include <memory>
#include <vector>

using std::vector;
using Tile_ptr = std::shared_ptr<ITile>;

class Level
{
	public:
	Level() = default;

	void Draw();
	void Reset();

	void SetTileAt(const Tile_ptr& tile, const int x, const int y);
	Tile_ptr TileAt(const int x, const int y);

	private:
	int32_t height;
	int32_t length;
	vector<Tile_ptr> grid;
	// TODO: Vector of entity
	// TODO: Collision map
};
