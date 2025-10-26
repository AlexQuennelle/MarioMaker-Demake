#include "level.h"
#include "tile.h"
#include "utils.h"

#include <array>
#include <bit>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <vector>

Level::Level()
	: height(15), length(100), playerStartPos({.x = 5, .y = 0}),
	  name("My Level"),
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
Level::Level(const std::string& filepath)
{
	this->filepath = std::string(filepath);
	std::ofstream file{filepath};
}
Level::~Level()
{
	// NOTE: Remove when asset manager is merged.
	//UnloadImage(this->img);
	//UnloadImage(this->sprites);
	UnloadTexture(this->tex);
}

vector<byte> Level::Serialize() const
{
	std::array<byte, 4> intBuffer{};
	// Metadata/Header
	vector<byte> bytes{'L', 'V', 'L', 0};
	// Level size
	InsertAsBytes(bytes, this->length);
	InsertAsBytes(bytes, this->height);
	// Player start position
	InsertAsBytes(bytes, static_cast<uint32_t>(this->playerStartPos.x));
	InsertAsBytes(bytes, static_cast<uint32_t>(this->playerStartPos.y));

	// Name
	InsertAsBytes(bytes, static_cast<uint32_t>(this->name.length()));
	for (auto ch : this->name)
	{
		bytes.push_back(ch);
	}
	// Pad bytes for alignment
	int alignment{
		static_cast<int>((((bytes.size() / 4) + 1) * 4) - bytes.size())};
	for (int i{0}; i < alignment; i++)
	{
		bytes.push_back(0);
	}

	// Run length encoding
	Tile currTile{this->grid[0]};
	uint32_t run{0};
	for (int i{0}; i < this->grid.size(); i++)
	{
		if (currTile != this->grid[i])
		{
			InsertAsBytes(bytes, run);
			InsertAsBytes(bytes, currTile);
			bytes.push_back(0);
			bytes.push_back(0);

			currTile = this->grid[i];
			run = 0;
		}
		run++;
	}
	InsertAsBytes(bytes, run);
	InsertAsBytes(bytes, currTile);
	bytes.push_back(0);
	bytes.push_back(0);

	return bytes;
}
template <typename T> void Level::InsertAsBytes(vector<byte>& vec, T data)
{
	std::array<byte, sizeof(data)> buffer{};
	std::memcpy(&buffer, &data, sizeof(data));
	for (auto byte : buffer)
	{
		vec.push_back(byte);
	}
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
			int i = (y * this->length) + x;
			if (TileAt(x, y).ID == TileID::ground && !visited[i])
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
		if (TileAt(x, y).ID == TileID::ground && !visited[i])
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
			int i = (h * this->length) + w;
			canExpand &= (TileAt(x, y).ID == TileID::ground && !visited[i]);
		}
		if (canExpand)
		{
			for (int w{x}; w < rWidth; w++)
			{
				int i = (h * this->length) + w;
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
			if (TileAt(x, y).ID == TileID::ground)
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
				static_cast<byte>(this->TileAt(x + i, y + j).ID ==
								  TileID::ground),
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

void Level::SetTileAt(const TileID tile, const int x, const int y,
					  const uint8_t flags)
{
	if (x >= 0 && x <= this->length - 1 && y >= 0 && y <= this->height - 1)
		grid[(y * this->length) + x] = Tile{.ID = tile, .flags = flags};
#ifndef NDEBUG
	else
	{
		SetTextColor(WARNING);
		std::cout << "WARNING: Attempted to set tile out of bounds";
		ClearStyles();
	}
#endif // !NDEBUG
}
Tile Level::TileAt(const int x, const int y)
{
	if (x >= 0 && x <= this->length - 1 && y >= 0 && y <= this->height - 1)
	{
		return grid[(y * this->length) + x];
	}
	else
	{
		return Tile{.ID = TileID::ground, .flags = 0};
	}
}

void Level::Reset() {}
