#include "level.h"
#include "tile.h"

Level::Level() : height(15), length(100)
{
	this->grid.resize(this->height * this->length);
	// HACK: This is temporary to fill in the ground
	for (int x{0}; x < length; x++)
	{
		for (int y{0}; y < height; y++)
		{
			if (y > 10)
				this->SetTileAt(TileID::ground, x, y);
			else
				this->SetTileAt(TileID::air, x, y);
		}
	}
}

void Level::SetTileAt(const TileID tile, const int x, const int y)
{
	grid[(x * this->height) + y] = tile;
}
TileID Level::TileAt(const int x, const int y)
{
	return grid[(x * this->height) + y];
}
