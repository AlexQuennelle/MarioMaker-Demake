#include "level.h"
#include "tile.h"
#include "utils.h"

#include <bit>
#include <cstdint>
#include <vector>

Level::Level() : height(15), length(100)
{
	this->grid.resize(this->height * this->length);
	// HACK: This is temporary to fill in the ground
	for (int x{0}; x < this->length; x++)
	{
		for (int y{0}; y < this->height; y++)
		{
			if (y > 10)
				this->SetTileAt(TileID::ground, x, y);
			else
				this->SetTileAt(TileID::air, x, y);
		}
	}
	this->GenCollisionMap();
	this->StitchTexture();
}

void Level::GenCollisionMap()
{
	std::vector<bool> visited{};
	visited.resize(this->height * this->length);

	for (int x{0}; x < this->length; x++)
	{
		for (int y{0}; y < this->height; y++)
		{
			int i = (x * this->height) + y;
			if (TileAt(x, y) == TileID::ground && !visited[i])
				this->colliders.push_back(GenCollisionRect(x, y, visited));
		}
	}
}
CollisionRect Level::GenCollisionRect(const int x, const int y,
									  vector<bool>& visited)
{
	int rWidth{0};
	for (int w{x}; w < this->length; w++)
	{
		int i = (w * this->height) + y;
		if (TileAt(x, y) == TileID::ground && !visited[i])
		{
			visited[i] = true;
			rWidth++;
		}
		else
			break;
	}

	int rHeight{1};
	for (int h{y + 1}; h < this->height; h++)
	{
		bool canExpand{true};
		for (int w{x}; w < rWidth; w++)
		{
			int i = (w * this->height) + h;
			canExpand &= (TileAt(x, y) == TileID::ground && !visited[i]);
		}
		if (canExpand)
		{
			for (int w{x}; w < rWidth; w++)
			{
				int i = (w * this->height) + h;
				visited[i] = true;
			}
			rHeight++;
		}
		else
			break;
	}

	return {
		.position = {static_cast<float>(x), static_cast<float>(y)},
		.size = {static_cast<float>(rWidth), static_cast<float>(rHeight)},
	};
}
void Level::StitchTexture()
{
	for (int x{0}; x < this->length; x++)
	{
		for (int y{0}; y < this->height; y++)
		{
			MarchSquares(x, y);
		}
	}
}
byte Level::MarchSquares(const int x, const int y)
{
	byte mask{0};
	for (const int i : {-1, 0, 1})
	{
		for (const int j : {-1, 0, 1})
		{
			if (i == x && j == y)
				continue;

			auto val{
				static_cast<byte>(this->TileAt(x + i, y + j) == TileID::ground),
			};
			mask |= std::rotl(val, (i * 3) + j);
		}
	}

	return mask;
}

void Level::SetTileAt(const TileID tile, const int x, const int y)
{
	grid[(x * this->height) + y] = tile;
}
TileID Level::TileAt(const int x, const int y)
{
	if (x >= 0 && x <= this->length - 1 && y >= 0 && y <= this->height - 1)
	{
		return grid[(x * this->height) + y];
	}
	else
	{
		return TileID::ground;
	}
}
