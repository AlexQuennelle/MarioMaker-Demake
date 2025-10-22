#pragma once

#include "tile.h"
#include "utils.h"

#include <array>
#include <cstdint>
#include <raylib.h>
#include <vector>

using std::array;
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
	void GenCollisionMap();
	CollisionRect GenCollisionRect(const int x, const int y,
								   vector<bool>& visited);
	void StitchTexture();
	byte MarchSquares(const int x, const int y);
	static array<Rectangle, 4> GetRects(const byte mask);

	int32_t height;
	int32_t length;
	vector<TileID> grid;
	vector<CollisionRect> colliders;
	Image img;
	Image sprites;
	Texture tex;
	// TODO: Vector of entity
};
