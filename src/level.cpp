#include "level.h"
#include "assetmanager.h"
#include "constants.h"
#include "entity.h"
#include "jumpingFireEnemy.h"
#include "powerup.h"
#include "tile.h"
#include "utils.h"
#include "walkerEnemy.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <random>
#include <ranges>
#include <raylib.h>
#include <span>
#include <string>
#include <utility>
#include <vector>

#ifndef NDEBUG
//#define DRAW_COLS
//#define LOG_LEVEL_DATA
#ifdef LOG_LEVEL_DATA
#include <format>
#endif // LOG_LEVEL_DATA
#endif // !NDEBUG

Level::Level(const std::string& filepath, AssetManager& am, float gravity) :
	am(am),
	filepath(filepath),
	img(),
	sprites(am.groundTiles),
	entities(0),
	tex(),
	gravity(gravity),
	saved(true)
{
#ifndef NDEBUG
	SetTextColor(INFO);
	std::cout << "New level from file\n";
	ClearStyles();
#endif // !NDEBUG
	namespace fs = std::filesystem;
	using std::ios;

	std::ifstream file{filepath.c_str(), ios::binary | ios::ate};

	if (file.is_open())
	{
		auto fSize = static_cast<size_t>(fs::file_size(filepath));
		std::vector<char> data(fSize, 0);

		file.seekg(0, ios::beg);
		file.read(data.data(), static_cast<int64_t>(fSize));
		file.close();

		std::string fileID(3, 0);
		std::memcpy(fileID.data(), data.data(), 3);

		if (fileID == "LVL")
		{
#ifdef LOG_LEVEL_DATA
			SetTextColor(SUCCESS);
			std::cout << "Valid level file found\n";
			ClearStyles();
#endif // !LOG_LEVEL_DATA

			this->ParseData(data);

			this->PopulateLevel();
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
Level::Level(AssetManager& am, std::string name, float gravity) :
	am(am),
	name(std::move(name)),
	img(nullptr),
	sprites(am.groundTiles),
	grid(static_cast<size_t>(DEFAULT_LEVEL_LENGTH * DEFAULT_LEVEL_HEIGHT)),
	tex(0),
	playerStartPos({.x = 2.0f, .y = DEFAULT_LEVEL_HEIGHT - 2.0f}),
	gravity(gravity),
	height(DEFAULT_LEVEL_HEIGHT),
	length(DEFAULT_LEVEL_LENGTH)
{
#ifndef NDEBUG
	SetTextColor(INFO);
	std::cout << "New level\n";
	ClearStyles();
#endif
	for (uint32_t x{0}; x < this->length; x++)
	{
		for (uint32_t y{0}; y < this->height; y++)
		{
			if (y >= this->height - 2)
				this->SetTileAt(TileID::ground, x, y);
		}
	}

	this->PopulateLevel();
	this->StitchTexture();
}
Level::~Level()
{
#ifndef NDEBUG
	SetTextColor(INFO);
	std::cout << "Deleted Level\n";
	ClearStyles();
#endif
	UnloadImage(this->img);
	this->img
		= {.data = nullptr, .width = 0, .height = 0, .mipmaps = 0, .format = 0};
	UnloadTexture(this->tex);
	this->tex.id = 0;
}

auto Level::Serialize() const -> vector<byte>
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
	for (auto character : this->name)
	{
		bytes.push_back(static_cast<byte>(character));
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
	for (uint64_t i{0}; i < this->grid.size(); i++)
	{
		if (currTile != this->grid[i])
		{
			InsertAsBytes(bytes, run);
			InsertAsBytes(bytes, currTile);

			currTile = this->grid[i];
			run = 0;
		}
		run++;
	}
	InsertAsBytes(bytes, run);
	InsertAsBytes(bytes, currTile);

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

void Level::Update()
{
	this->toggledThisFrame = false;
	for (auto& entity : this->spawnQueue)
	{
		this->entities.push_back(std::move(entity));
	}
	this->spawnQueue.clear();
	vector<Rectangle> solidCols = this->GetSolidEntityColliders();
	vector<Rectangle> levelCols = this->GetColliders();

	solidCols.reserve(solidCols.size() + levelCols.size());

	solidCols.insert(solidCols.end(), levelCols.begin(), levelCols.end());

	for (const auto& entity : this->entities)
	{
		if (entity->IsActive())
		{
			HandleRequest(entity->Update(solidCols));
		}
	}
	for (auto* toggle : this->toggleBlocks)
	{
		toggle->SetState(this->toggleState);
	}
}
void Level::Draw()
{
	DrawTexture(this->tex, 0, 0, WHITE);
	for (const auto& entity : this->entities)
	{
		if (entity->IsActive())
		{
			entity->Draw();
		}
	}
#ifdef DRAW_COLS
	for (auto rec : this->colliders)
	{
		DrawRectangleLinesEx(
			{rec.x * 16, rec.y * 16, rec.width * 16, rec.height * 16}, 1.0f,
			{0, 200, 255, 170});
	}
#endif // DRAW_COLS
}
void Level::EditDraw()
{
	DrawTexture(this->tex, 0, 0, WHITE);
	for (const auto& entity : this->entities)
	{
		if (entity->IsActive())
		{
			entity->EditDraw();
		}
	}
}
void Level::DrawGrid(RenderTexture& tex)
{
	BeginTextureMode(tex);
	ClearBackground(BLANK);
	for (uint32_t x{0}; x < this->length; x++)
	{
		for (uint32_t y{0}; y < this->height; y++)
		{
			DrawRectangleLinesEx({static_cast<float>(x) * 16.0f,
								  static_cast<float>(y) * 16.0f, 16.0f, 16.0f},
								 0.5f, Fade(WHITE, 0.8f));
		}
	}
	EndTextureMode();
}

void Level::PopulateLevel()
{
	std::vector<bool> visited{};
	// HACK: Find a better way to deal with this warning
	visited.resize(this->height * this->length); // NOLINT

	for (uint32_t x{0}; x < this->length; x++)
	{
		for (uint32_t y{0}; y < this->height; y++)
		{
			auto i = (y * this->length) + x;
			Tile currTile{this->TileAt(x, y)};
			if (currTile.ID == TileID::ground)
			{
				if (!visited[i])
				{
					this->colliders.push_back(
						this->GenCollisionRect(x, y, visited));
				}
			}
			else if (currTile.ID != TileID::air)
			{
				this->SpawnEntity(x, y, this->TileAt(x, y));
			}
		}
	}
}
auto Level::GenCollisionRect(const uint32_t x, const uint32_t y,
							 vector<bool>& visited) -> Rectangle
{
	uint32_t rWidth{0};
	for (uint32_t w{x}; w < this->length; w++)
	{
		uint32_t i = (y * this->length) + w;
		if ((TileAt(w, y).ID == TileID::ground) && !visited[i])
		{
			visited[i] = true;
			rWidth++;
		}
		else
			break;
	}

	uint32_t rHeight{1};
	for (uint32_t h{y + 1}; h < this->height; h++)
	{
		bool canExpand{true};
		for (uint32_t w{x}; w < rWidth + x; w++)
		{
			uint32_t i = (h * this->length) + w;
			canExpand &= ((TileAt(w, h).ID == TileID::ground) && !visited[i]);
		}

		if (canExpand)
		{
			for (uint32_t w{x}; w < rWidth + x; w++)
			{
				uint32_t i = (h * this->length) + w;
				visited[i] = true;
			}
			rHeight++;
		}
		else
			break;
	}

#ifdef LOG_LEVEL_DATA
	std::cout << std::format("Rect: [({}, {}) -> {} x {}]\n", x, y, rWidth,
							 rHeight);
#endif // !LOG_LEVEL_DATA

	return {
		static_cast<float>(x),
		static_cast<float>(y),
		static_cast<float>(rWidth),
		static_cast<float>(rHeight),
	};
}
void Level::HandleRequest(EntityReq request)
{
	switch (request.index())
	{
	case 1: // Spawn requested Entity
		this->spawnQueue.push_back(std::move(std::get<1>(request).entity));
		break;
	case 2: // Toggle state of toggle tiles
		if (!this->toggledThisFrame)
		{
			this->toggleState = !this->toggleState;
			this->toggledThisFrame = true;
		}
		break;
	default:
		break;
	}
}
void Level::StitchTexture()
{
	this->img
		= {.data = nullptr, .width = 0, .height = 0, .mipmaps = 0, .format = 0};
	this->img = GenImageColor(static_cast<int32_t>(this->length * 16),
							  static_cast<int32_t>(this->height * 16), BLANK);
	for (uint32_t y{0}; y < this->height; y++)
	{
		for (uint32_t x{0}; x < this->length; x++)
		{
			if (TileAt(x, y).ID == TileID::ground)
			{
				byte tileMask{this->MarchSquares(x, y)};
				array<Rectangle, 4> rects{this->GetRects(tileMask)};

				for (uint32_t i{0}; i < 4; i++)
				{
					Rectangle dest{
						static_cast<float>((x * 16) + ((i % 2) * 8)),
						static_cast<float>(y * 16)
							+ (std::floor(static_cast<float>(i) / 2.0f) * 8),
						8.0f,
						8.0f,
					};
					// NOLINTNEXTLINE
					ImageDraw(&this->img, this->sprites, rects[i], dest, WHITE);
				}
			}
		}
	}
	this->tex = LoadTextureFromImage(this->img);
}
auto Level::MarchSquares(const uint32_t x, const uint32_t y) -> byte
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
	for (const uint32_t i : {-1u, 0u, 1u})
	{
		for (const uint32_t j : {-1u, 0u, 1u})
		{
			if (x + i == x && y + j == y)
				continue;

			auto val{
				static_cast<byte>(this->TileAt(x + i, y + j).ID
								  == TileID::ground),
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
#ifdef LOG_LEVEL_DATA
	std::cout << std::format("Level size: {} x {}\n", this->length,
							 this->height);
#endif // !LOG_LEVEL_DATA

	uint32_t playStartX{0};
	uint32_t playStartY{0};
	std::memcpy(&playStartX, &data[12], 4);
	std::memcpy(&playStartY, &data[16], 4);
	this->playerStartPos = {
		.x = static_cast<float>(playStartX),
		.y = static_cast<float>(playStartY),
	};

#ifdef LOG_LEVEL_DATA
	std::cout << std::format("Player start: ({}, {})\n", this->playerStartPos.x,
							 this->playerStartPos.y);
#endif // !LOG_LEVEL_DATA

	uint32_t nameLen{0};
	std::memcpy(&nameLen, &data[20], 4);
	this->name = std::string(nameLen, 0);
	std::memcpy(this->name.data(), &data[24], nameLen);
#ifdef LOG_LEVEL_DATA
	std::cout << "Level name: " << this->name << '\n';
#endif // !LOG_LEVEL_DATA

	struct DataBlock
	{
		uint32_t num;
		Tile dat;
	};

	auto beg(data.begin() + (24 + (((nameLen / 4) + 1) * 4)));
	std::span<const char> span(beg, data.end());
	namespace r = std::ranges;
	namespace rv = std::ranges::views;
	auto fuse = [](auto range) -> DataBlock
	{
		DataBlock block{};
		std::memcpy(&block, range.data(), 8);
		return block;
	};
	this->grid.reserve(this->length * this->height); // NOLINT
#ifndef __EMSCRIPTEN__
	this->grid
		= rv::chunk(span, 8)
		  | rv::transform(fuse) // NOLINTNEXTLINE
		  | rv::transform([](auto blk) { return rv::repeat(blk.dat, blk.num); })
		  | rv::join
		  | r::to<vector<Tile>>();
#else
	auto spanStart = std::bit_cast<const DataBlock*>(
			(data.begin() + (24 + (((nameLen / 4) + 1) * 4))).base());
	auto spanEnd = std::bit_cast<const DataBlock*>(data.end().base());
	this->grid
		= std::span<const DataBlock>(spanStart, spanEnd - spanStart)
		| rv::transform([](auto blk) { return rv::repeat(blk.dat, blk.num); })
		| rv::join
		| r::to<vector<Tile>>();
#endif // !__EMSCRIPTEN__
}

auto Level::GetRects(const byte mask) -> array<Rectangle, 4>
{
	std::random_device randomDev{};
	std::seed_seq seeds({randomDev(), randomDev()});
	std::mt19937 mt(seeds);
	std::uniform_int_distribution<> rand2(0, 1);
	std::uniform_int_distribution<> rand4(0, 3);
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
			.y = static_cast<float>(rand2(mt) * 8),
			// .y = static_cast<float>(rand(mt) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 2) == 0)
	{
		topL = {
			.x = 24.0f,
			.y = static_cast<float>(rand2(mt) * 8),
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
			.y = static_cast<float>(rand2(mt) * 8),
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
			.y = static_cast<float>(rand2(mt) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 64) == 0)
	{
		topR = {
			.x = 24.0f,
			.y = static_cast<float>(rand2(mt) * 8) + 16.0f,
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
			.y = static_cast<float>(rand4(mt) * 8),
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
			.y = static_cast<float>(rand2(mt) * 8) + 16,
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 2) == 0)
	{
		botL = {
			.x = 24.0f,
			.y = static_cast<float>(rand2(mt) * 8),
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
			.y = static_cast<float>(rand4(mt) * 8),
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
			.y = static_cast<float>(rand2(mt) * 8) + 16.0f,
			.width = 8.0f,
			.height = 8.0f,
		};
	}
	else if ((mask & 64) == 0)
	{
		botR = {
			.x = 24.0f,
			.y = static_cast<float>(rand2(mt) * 8) + 16.0f,
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
			.y = static_cast<float>(rand4(mt) * 8),
			.width = 8.0f,
			.height = 8.0f,
		};
	}

	return {topL, topR, botL, botR};
}

void Level::SetTileAt(const TileID tile, const uint32_t x, const uint32_t y,
					  const uint16_t flags)
{
	if (x >= 0 && x <= this->length - 1 && y >= 0 && y <= this->height - 1)
	{
		this->saved = false;
		grid[(y * this->length) + x] = Tile{.ID = tile, .flags = flags};
	}
#ifndef NDEBUG
	else
	{
		SetTextColor(WARNING);
		std::cout << "WARNING: Attempted to set tile out of bounds";
		ClearStyles();
	}
#endif // !LOG_LEVEL_DATA
}
void Level::SetTileAt(const TileID tile, const Vector2Int pos,
					  const uint16_t flags)
{
	this->SetTileAt(tile, static_cast<uint32_t>(pos.x),
					static_cast<uint32_t>(pos.y), flags);
}
void Level::SetTileAtEditor(const TileID tile, const Vector2Int pos,
							const uint16_t flags)
{
	auto prevTile{this->TileAt(static_cast<uint32_t>(pos.x),
							   static_cast<uint32_t>(pos.y))};
	if (prevTile.ID == tile && prevTile.flags == flags)
		return;

	this->SetTileAt(tile, pos, flags);
	this->Reset();
}
auto Level::TileAt(const uint32_t x, const uint32_t y) -> Tile
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
void Level::SpawnEntity(const uint32_t x, const uint32_t y, const Tile basis)
{
	switch (basis.ID)
	{
		using enum TileID;
	case (brick):
		this->entities.push_back(std::make_unique<Block>(
			x, y, this->am, static_cast<bool>(basis.flags & 1)));
		break;
	case (spikes):
		this->entities.push_back(std::make_unique<Spike>(x, y, this->am));
		break;
	case (itemBox):
		this->entities.push_back(std::make_unique<ItemBox>(
			x, y, this->am, static_cast<bool>(basis.flags & 1),
			static_cast<bool>((basis.flags >> 1) & 1)));
		break;
	case (coin):
		this->entities.push_back(std::make_unique<Coin>(x, y, this->am));
		break;
	case (toggleSwitch):
		this->entities.push_back(
			std::make_unique<ToggleSwitch>(x, y, this->am));
		break;
	case (toggleBlock):
		this->entities.push_back(std::make_unique<ToggleBlock>(
			x, y, this->am, static_cast<bool>(basis.flags & 1)));
		break;
	case (mushroom):
		this->entities.push_back(
			std::make_unique<Mushroom>(x, y, this->am, gravity));
		break;
	case (walkerEnemy):
		this->entities.push_back(std::make_unique<WalkerEnemy>(
			x, y, this->am, gravity, static_cast<bool>(basis.flags & 1)));
		break;
	case (fireFlower):
		this->entities.push_back(
			std::make_unique<FireFlower>(x, y, this->am, gravity));
		break;
	case (podoboo):
		this->entities.push_back(
			std::make_unique<JumpingFireEnemy>(x, y, this->am, this->height));
		break;
	default:
		break;
	}
	if (basis.ID == TileID::toggleBlock || basis.ID == TileID::toggleSwitch)
	{
		auto* block{dynamic_cast<IToggleable*>(
			this->entities[this->entities.size() - 1].get())};
		if (block != nullptr)
		{
			this->toggleBlocks.push_back(block);
		}
	}
}

void Level::SetLevelSize(const uint32_t length, const uint32_t height)
{
	if ((this->length == length) && (this->height == height))
		return;

	this->saved = false;

	uint32_t overlapX{std::min(this->length, length)};
	uint32_t overlapY{std::min(this->height, height)};

	vector<Tile> oldGrid(this->grid);
	this->grid.clear();
	this->grid.resize(static_cast<size_t>(length) * height);

#ifdef LOG_LEVEL_DATA
	std::cout << length << " x " << height << '\n';
	std::cout << oldGrid.size() << " -> " << this->grid.size() << '\n';
#endif // !LOG_LEVEL_DATA

	for (uint32_t x{0}; x < overlapX; x++)
	{
		for (uint32_t y{0}; y <= overlapY; y++)
		{
			uint32_t i{((this->height - y) * this->length) + x};
			uint32_t j{((height - y) * length) + x};

			if (j < this->grid.size())
				this->grid[j] = oldGrid[i];
		}
	}

	this->length = length;
	this->height = height;
	this->Reset();
}

void Level::Reset()
{
	this->colliders.clear();
	this->entities.clear();
	this->toggleBlocks.clear();
	this->toggleState = false;
	this->PopulateLevel();
	UnloadImage(this->img);
	this->img
		= {.data = nullptr, .width = 0, .height = 0, .mipmaps = 0, .format = 0};
	UnloadTexture(this->tex);
	this->tex = {.id = 0, .width = 0, .height = 0, .mipmaps = 0, .format = 0};
	this->StitchTexture();
}

auto Level::GetEntities() -> vector<Entity*>
{
	vector<Entity*> entities;
	for (Entity_ptr& entity : this->entities)
	{
		if (entity->IsActive())
		{
			entities.push_back(entity.get());
		}
	}
	return entities;
}

auto Level::GetSolidEntityColliders() -> vector<Rectangle>
{
	vector<Rectangle> solids;
	for (Entity_ptr& entity : this->entities)
	{
		if (entity->IsSolid() && entity->IsActive())
		{
			solids.push_back(entity->GetCollider());
		}
	}
	return solids;
}
