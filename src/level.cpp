#include "level.h"
#include "assetmanager.h"
#include "tile.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <bit>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>

#ifndef NDEBUG
//#define DRAW_COLS
#endif // !NDEBUG

//Level::Level()
//	: height(15), length(100), playerStartPos({.x = 5, .y = 0}),
//	  name("My Level")
//{
//	std::srand(std::time({}));
//	this->grid.resize(this->height * this->length);
//	// HACK: This is temporary to fill in the ground
//	for (int x{0}; x < this->length; x++)
//	{
//		for (int y{0}; y < this->height; y++)
//		{
//			if (y > 10)
//				this->SetTileAt(TileID::ground, x, y);
//			else
//				this->SetTileAt(TileID::air, x, y);
//		}
//	}
//	this->GenCollisionMap();
//
//	this->img = GenImageColor(this->length * 16, this->height * 16, BLANK);
//	this->tex = LoadTextureFromImage(this->img);
//	this->StitchTexture();
//}
Level::Level(const std::string& filepath, AssetManager* am)
	: sprites(am->groundTiles)
{
	namespace fs = std::filesystem;
	using std::ios;

	std::srand(std::time({}));

	this->filepath = std::string(filepath);

	std::ifstream file{filepath.c_str(), ios::binary | ios::ate};

	if (file.is_open())
	{
		std::streampos fSize = fs::file_size(filepath);
		std::vector<char> data(fSize, 0);

		file.seekg(0, ios::beg);
		file.read(data.data(), fSize);
		file.close();

		std::string fileID(3, 0);
		std::memcpy(fileID.data(), data.data(), 3);

		if (fileID == "LVL")
		{
#ifndef NDEBUG
			SetTextColor(SUCCESS);
			std::cout << "Valid level file found\n";
			ClearStyles();
#endif // !NDEBUG

			this->ParseData(data);

			this->GenCollisionMap();

			this->img =
				GenImageColor(this->length * 16, this->height * 16, BLANK);
			this->tex = LoadTextureFromImage(this->img);
			this->StitchTexture();
		}
	}
	else
	{
		SetTextColor(ERROR);
		std::cerr << "ERROR: Failed to open file at " << filepath << '\n';
		ClearStyles();
	}
}
Level::~Level()
{
	// NOTE: Remove when asset manager is merged.
	//UnloadImage(this->img);
	//UnloadImage(this->sprites);
	// BUG: Commenting this line causes a memory leak, but uncommenting it means
	// texturs no longer load correctly
	// UnloadTexture(this->tex);
}

vector<byte> Level::Serialize() const
{
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

void Level::Draw()
{
	DrawTexture(this->tex, 0, 0, WHITE);
#ifdef DRAW_COLS
	for (auto rec : this->colliders)
	{
		DrawRectangleLinesEx(
			{rec.x * 16, rec.y * 16, rec.width * 16, rec.height * 16}, 1.0f,
			{0, 200, 255, 170});
	}
#endif // DRAW_COLS
}
void Level::DrawGrid(RenderTexture& tex)
{
	BeginTextureMode(tex);
	ClearBackground(BLANK);
	for (int x{0}; x < this->length; x++)
	{
		for (int y{0}; y < this->height; y++)
		{
			DrawRectangleLinesEx({x * 16.0f, y * 16.0f, 16.0f, 16.0f}, 0.5f,
								 Fade(WHITE, 0.8f));
		}
	}
	EndTextureMode();
}

void Level::GenCollisionMap()
{
	std::vector<bool> visited{};
	visited.resize(this->height * this->length);

	for (int x{0}; x < this->length; x++)
	{
		for (int y{0}; y < this->height; y++)
		{
			int i = (y * this->length) + x;
			if (x == 17 && y == 2)
			{
				std::cout << (int)TileAt(x, y).ID << '\n';
				std::cout << i << '\n';
				std::cout << visited[i] << '\n';
			}
			if (TileAt(x, y).ID == TileID::ground && !visited[i])
				this->colliders.push_back(GenCollisionRect(x, y, visited));
		}
	}
}
Rectangle Level::GenCollisionRect(const int x, const int y,
								  vector<bool>& visited)
{
	int rWidth{0};
	for (int w{x}; w < this->length; w++)
	{
		int i = (y * this->length) + w;
		if ((TileAt(w, y).ID == TileID::ground) && !visited[i])
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
		for (int w{x}; w < rWidth + x; w++)
		{
			int i = (h * this->length) + w;
			canExpand &= ((TileAt(w, h).ID == TileID::ground) && !visited[i]);
		}

		if (canExpand)
		{
			for (int w{x}; w < rWidth + x; w++)
			{
				int i = (h * this->length) + w;
				visited[i] = true;
			}
			rHeight++;
		}
		else
			break;
	}

#ifndef NDEBUG
	std::cout << std::format("Rect: [({}, {}) -> {} x {}]\n", x, y, rWidth,
							 rHeight);
#endif // !NDEBUG

	return {
		static_cast<float>(x),
		static_cast<float>(y),
		static_cast<float>(rWidth),
		static_cast<float>(rHeight),
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
void Level::ParseData(const vector<char>& data)
{
	std::memcpy(&this->length, &data[4], 4);
	std::memcpy(&this->height, &data[8], 4);
#ifndef NDEBUG
	std::cout << std::format("Level size: {} x {}\n", this->length,
							 this->height);
#endif // !NDEBUG

	uint32_t playStartX{0};
	uint32_t playStartY{0};
	std::memcpy(&playStartX, &data[12], 4);
	std::memcpy(&playStartY, &data[16], 4);
	this->playerStartPos = {
		.x = static_cast<float>(playStartX),
		.y = static_cast<float>(playStartY),
	};

#ifndef NDEBUG
	std::cout << std::format("Player start: ({}, {})\n", this->playerStartPos.x,
							 this->playerStartPos.y);
#endif // !NDEBUG

	uint32_t nameLen{0};
	std::memcpy(&nameLen, &data[20], 4);
	this->name = std::string(nameLen, 0);
	std::memcpy(this->name.data(), &data[24], nameLen);
#ifndef NDEBUG
	std::cout << "Level name: " << this->name << '\n';
#endif // !NDEBUG

	const char* addr{&data[24 + (((nameLen / 4) + 1) * 4)]};
	const char* endAddr{data.data() + data.size() - 1};

	this->grid.reserve(this->length * this->height);
	while (addr < endAddr)
	{
		uint32_t runLength{0};
		std::memcpy(&runLength, addr, 4);
		Tile tile{.ID = TileID::air, .flags = 0};
		std::memcpy(&tile, addr + 4, 2);
		for (int i{0}; i < runLength; i++)
		{
			this->grid.push_back(tile);
		}
		addr += 8;
	}
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
		botL = {.x = 0.0f, .y = 24.0f, .width = 8.0f, .height = 8.0f};
	}
	else if ((mask & 16) == 0)
	{
		botL = {
			.x = 16.0f,
			.y = static_cast<float>((std::rand() % 2) * 8) + 16,
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 2) == 0)
	{
		botL = {
			.x = 24.0f,
			.y = static_cast<float>((std::rand() % 2) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 4) == 0)
	{
		botL = {.x = 8.0f, .y = 24.0f, .width = 8.0f, .height = 8.0f};
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
		botR = {.x = 0.0f, .y = 16.0f, .width = 8.0f, .height = 8.0f};
	}
	else if ((mask & 16) == 0)
	{
		botR = {
			.x = 16.0f,
			.y = static_cast<float>((std::rand() % 2) * 8) + 16.0f,
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
		botR = {.x = 8.0f, .y = 16.0f, .width = 8.0f, .height = 8.0f};
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
void Level::SetTileAt(const TileID tile, const Vector2Int pos,
					  const uint8_t flags)
{
	this->SetTileAt(tile, pos.x, pos.y, flags);
}
void Level::SetTileAtEditor(const TileID tile, const Vector2Int pos,
							const uint8_t flags)
{
	auto prevTile{this->TileAt(pos.x, pos.y)};
	if (prevTile.ID == tile && prevTile.flags == flags)
		return;

	this->SetTileAt(tile, pos, flags);
	this->StitchTexture();
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

void Level::SetLevelSize(const int length, const int height)
{
	if ((this->length == length) && (this->height == height))
	{
#ifndef NDEBUG
		std::cout << "Early out.\n";
#endif // !NDEBUG
		return;
	}

	int overlapX{std::min(this->length, length)};
	int overlapY{std::min(this->height, height)};

	vector<Tile> oldGrid(this->grid);
	this->grid.clear();
	this->grid.resize(length * height);

#ifndef NDEBUG
	std::cout << length << " x " << height << '\n';
	std::cout << oldGrid.size() << " -> " << this->grid.size() << '\n';
#endif // !NDEBUG

	for (int x{0}; x < overlapX; x++)
	{
		for (int y{0}; y < overlapY; y++)
		{
			int i{((this->height - y) * this->length) + x};
			int j{((height - y) * length) + x};

			if (j < this->grid.size())
				this->grid[j] = oldGrid[i];
		}
	}

	this->length = length;
	this->height = height;
	this->img = GenImageColor(this->length * 16, this->height * 16, BLANK);
	this->tex = LoadTextureFromImage(this->img);
	this->StitchTexture();
}

void Level::Reset() {}
