#include "level.h"
#include "tile.h"
#include "utils.h"

#include <bit>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <raylib.h>
#include <vector>

Level::Level()
	: height(15), length(100),
	  // FIX: This should be moved to a resource manager
	  sprites(LoadImage(RESOURCES_PATH "sprites/groundSprites.png"))
{
	std::srand(std::time({}));
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

	this->img = GenImageColor(this->length * 16, this->height * 16, BLANK);
	this->tex = LoadTextureFromImage(this->img);
	this->StitchTexture();
}

void Level::Draw() { DrawTexture(this->tex, 0, 0, WHITE); }

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
	this->img = GenImageColor(this->length * 16, this->height * 16, BLANK);
	for (int y{0}; y < this->height; y++)
	{
		for (int x{0}; x < this->length; x++)
		{
			if (TileAt(x, y) == TileID::ground)
			{
				byte tileMask{this->MarchSquares(x, y)};
				array<Rectangle, 4> rects{this->GetRects(tileMask)};

				for (int i{0}; i < 4; i++)
				{
					Rectangle dest{
						(x * 16.0f) + ((i % 2) * 8.0f),
						(y * 16.0f) + ((i / 2) * 8.0f),
						8.0f,
						8.0f,
					};
					ImageDraw(&this->img, this->sprites, rects[i], dest, WHITE);
				}
			}
		}
	}
	UpdateTexture(this->tex, this->img.data);
}
byte Level::MarchSquares(const int x, const int y)
{
	byte mask{0};
	int shift{0};
	/* Bit encoding pattern:
	 * 1 4 6
	 * 2 x 7
	 * 3 5 8
	 *
	 * ==
	 *
	 * 87654321
	 */
	for (const int i : {-1, 0, 1})
	{
		for (const int j : {-1, 0, 1})
		{
			if (x + i == x && y + j == y)
				continue;

			auto val{
				static_cast<byte>(this->TileAt(x + i, y + j) == TileID::ground),
			};
			mask |= std::rotl(val, shift);
			shift++;
		}
	}

	return mask;
}
array<Rectangle, 4> Level::GetRects(const byte mask)
{
	// Top left
	Rectangle topL;
	if ((mask & 10) == 0)
	{
		topL = {.x = 0.0f, .y = 0.0f, .width = 8.0f, .height = 8.0f};
	}
	else if ((mask & 8) == 0)
	{
		topL = {
			.x = 16.0f,
			.y = static_cast<float>((std::rand() % 2) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 2) == 0)
	{
		topL = {
			.x = 24.0f,
			.y = static_cast<float>((std::rand() % 2) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 1) == 0)
	{
		topL = {.x = 8.0f, .y = 0.0f, .width = 8.0f, .height = 8.0f};
	}
	else
	{
		topL = {
			.x = 32.0f,
			.y = static_cast<float>((std::rand() % 4) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}

	// Top right
	Rectangle topR;
	if ((mask & 72) == 0)
	{
		topR = {.x = 0.0f, .y = 8.0f, .width = 8.0f, .height = 8.0f};
	}
	else if ((mask & 8) == 0)
	{
		topR = {
			.x = 16.0f,
			.y = static_cast<float>((std::rand() % 2) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 64) == 0)
	{
		topR = {
			.x = 24.0f,
			.y = static_cast<float>((std::rand() % 2) * 8) + 16.0f,
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 32) == 0)
	{
		topR = {.x = 8.0f, .y = 8.0f, .width = 8.0f, .height = 8.0f};
	}
	else
	{
		topR = {
			.x = 32.0f,
			.y = static_cast<float>((std::rand() % 4) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}

	// Bottom left
	Rectangle botL;
	if ((mask & 18) == 0)
	{
		botL = {.x = 0.0f, .y = 8.0f, .width = 8.0f, .height = 8.0f};
	}
	else if ((mask & 16) == 0)
	{
		botL = {
			.x = 16.0f,
			.y = static_cast<float>((std::rand() % 2) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 2) == 0)
	{
		botL = {
			.x = 24.0f,
			.y = static_cast<float>((std::rand() % 2) * 8) + 16.0f,
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 4) == 0)
	{
		botL = {.x = 8.0f, .y = 8.0f, .width = 8.0f, .height = 8.0f};
	}
	else
	{
		botL = {
			.x = 32.0f,
			.y = static_cast<float>((std::rand() % 4) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}

	// Bottom right
	Rectangle botR;
	if ((mask & 80) == 0)
	{
		botR = {.x = 0.0f, .y = 8.0f, .width = 8.0f, .height = 8.0f};
	}
	else if ((mask & 16) == 0)
	{
		botR = {
			.x = 16.0f,
			.y = static_cast<float>((std::rand() % 2) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 64) == 0)
	{
		botR = {
			.x = 24.0f,
			.y = static_cast<float>((std::rand() % 2) * 8) + 16.0f,
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 128) == 0)
	{
		botR = {.x = 8.0f, .y = 8.0f, .width = 8.0f, .height = 8.0f};
	}
	else
	{
		botR = {
			.x = 32.0f,
			.y = static_cast<float>((std::rand() % 4) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}

	return {topL, topR, botL, botR};
}

void Level::SetTileAt(const TileID tile, const int x, const int y)
{
	if (x >= 0 && x <= this->length - 1 && y >= 0 && y <= this->height - 1)
		grid[(x * this->height) + y] = tile;
#ifndef NDEBUG
	else
	{
		SetTextColor(WARNING);
		std::cout << "WARNING: Attempted to set tile out of bounds";
		ClearStyles();
	}
#endif // !NDEBUG
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
