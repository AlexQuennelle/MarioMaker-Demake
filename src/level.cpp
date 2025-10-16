#include "level.h"

void Level::SetTileAt(const Tile_ptr& tile, const int x, const int y)
{
	grid[(x * this->height) + y] = tile;
}
Tile_ptr Level::TileAt(const int x, const int y)
{
	return grid[(x * this->height) + y];
}
