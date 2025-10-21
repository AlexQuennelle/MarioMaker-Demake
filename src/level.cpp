#include "level.h"
#include "tile.h"
#include "utils.h"

#include <array>
#include <bit>
#include <raylib.h>
#include <vector>

Level::Level()
	: height(15), length(100),
	  sprites(LoadImage(RESOURCES_PATH "sprites/groundSprites.png"))
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

	this->img = GenImageColor(this->length * 16, this->height * 16, BLANK);
	this->tex = LoadTextureFromImage(this->img);
	this->StitchTexture();
}

void Level::Draw()
{
	DrawTexture(this->tex, 0, 0, WHITE);
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
	this->img = GenImageColor(this->length * 16, this->height * 16, BLANK);
	for (int y{0}; y < this->height; y++)
	{
		for (int x{0}; x < this->length; x++)
		{
			if (TileAt(x, y) == TileID::ground)
			{
				byte tileMask{this->MarchSquares(x, y)};
				std::array<Rectangle, 4> rects{this->GetRects(tileMask)};

				ImageDraw(&this->img, this->sprites, rects[0],
						  {x * 16.0f, y * 16.0f, 8.0f, 8.0f}, WHITE);
			}
		}
	}
	UpdateTexture(this->tex, this->img.data);
}
byte Level::MarchSquares(const int x, const int y)
{
	byte mask{0};
	int shift{0};
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
const std::array<Rectangle, 4> Level::GetRects(const byte mask)
{
	// Top left
	Rectangle topL;
	if ((mask & 10) == 0)
		topL = {.x = 0.0f, .y = 0.0f, .width = 8.0f, .height = 8.0f};
	else if ((mask & 8) == 0)
		topL = {.x = 16.0f, .y = 0.0f, .width = 8.0f, .height = 8.0f};
	else if ((mask & 2) == 0)
		topL = {.x = 24.0f, .y = 0.0f, .width = 8.0f, .height = 8.0f};
	else if ((mask & 1) == 0)
		topL = {.x = 8.0f, .y = 0.0f, .width = 8.0f, .height = 8.0f};
	else
		topL = {.x = 32.0f, .y = 0.0f, .width = 8.0f, .height = 8.0f};

	Rectangle topR;
	Rectangle botL;
	Rectangle botR;

	return {topL, topR, botL, botR};
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
