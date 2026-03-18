#pragma once

#include "assetmanager.h"
#include "entity.h"
#include "tile.h"
#include "utils.h"

#include <array>
#include <cstdint>
#include <memory>
#include <raylib.h>
#include <string>
#include <vector>

using std::array;
using std::vector;
/** @brief Alias for @link std::unique_ptr @endlink to @link Entity @endlink */
using Entity_ptr = std::unique_ptr<Entity>;

class Level
{
	public:
	Level() = delete;
	Level(const std::string& filepath, AssetManager& am, float gravity = 0);
	Level(AssetManager& am, std::string name, float gravity = 0);
	Level(const Level& other) = delete;
	Level(Level&& other) = default;

	~Level();

	auto operator=(const Level& other) -> Level& = delete;
	auto operator=(Level&& other) -> Level& = delete;

	[[nodiscard]] auto Serialize() const -> vector<byte>;

	void Update();
	void Draw();
	void EditDraw();
	void DrawGrid(RenderTexture& tex);
	/**
	 * @brief Rebuilds the level from the tile grid. This respawns all
	 *        entities.
	 */
	void Reset();
	void HandleRequest(EntityReq request);

	/**
	 * @brief Sets a tile in the grid at position (x, y). If that position is
	 *        outside the bounds of the level, this method fails.
	 *
	 * @param tile ID of the tile to be set
	 * @param x the x position to set the tile
	 * @param y the y position to set the tile
	 * @param flags any flags a tile should have
	 */
	void SetTileAt(const TileID tile, const uint32_t x, const uint32_t y,
				   const uint16_t flags = 0);
	void SetTileAt(const TileID tile, const Vector2Int pos,
				   const uint16_t flags = 0);
	void SetTileAtEditor(const TileID tile, const Vector2Int pos,
						 const uint16_t flags = 0);
	/**
	 * @param x the x position to query
	 * @param y the y position to query
	 *
	 * @returns The @link TileID @endlink in the grid at position (x, y).
	 * @warning if (x, y) is outside the bounds of the level, this method
	 *          returns a @link TileID::ground @endlink.
	 */
	auto TileAt(const uint32_t x, const uint32_t y) -> Tile;
	void SpawnEntity(const uint32_t x, const uint32_t y, const Tile basis);
	void SpawnEntityEditor(const uint32_t x, const uint32_t y,
						   const Tile basis);

	// Getters
	auto GetPlayerStartPos() const -> Vector2 { return playerStartPos; }
	auto GetColliders() const -> const vector<Rectangle>& { return colliders; }
	auto HasFilepath() const -> bool { return !this->filepath.empty(); }
	void SetFilepath(const std::string& path) { this->filepath = path; }
	auto GetFilepath() const -> const std::string& { return this->filepath; }
	auto GetLength() const -> uint32_t { return this->length; }
	auto GetHeight() const -> uint32_t { return this->height; }
	void SetLevelSize(const uint32_t length, const uint32_t height);
	auto GetName() const -> const std::string& { return this->name; }
	void SetName(const std::string& newName) { this->name = newName; }
	auto IsSaved() const -> bool { return this->saved; }
	void Save() { this->saved = true; }

	auto GetEntities() -> vector<Entity*>;

	auto GetSolidEntityColliders() -> vector<Rectangle>;

	private:
	/**
	 * @brief Populates the colliders vector with collision rectangles. These
	 *        are generated using a 2D greedy meshing algorithm to minimize the
	 *        amount of collision checks required.
	 */
	void PopulateLevel();
	/**
	 * @brief Generates a collision rectangle starting at (x, y) that expands
	 *        as much to the left and down as it can without encountering a cell
	 *        that has already been visited or that isn't ground.
	 * @note This is called by @link GenCollisionMap @endlink
	 *
	 * @note The visited parameter is mutated to reflect any newly visited
	 *       cells
	 *
	 * @param x Starting x position for the output rectangle.
	 * @param y Starting y position for the output rectangle.
	 * @param visited A vector that mirrors @link grid @endlink storing wich
	 *        cells have been visited by the greedy meshing.
	 *
	 * @returns a @link Rectangle @endlink
	 */
	auto GenCollisionRect(const uint32_t x, const uint32_t y,
						  vector<bool>& visited) -> Rectangle;
	/**
	 * @brief Stitches ground tile sprites into an image representing the
	 *        entire level using the data in @link grid @endlink. This is done
	 *        using marching squares algorithm to determine which sprite variant
	 *        should be used.
	 */
	void StitchTexture();
	/**
	 * @brief Performs the marching squares algorithm on the tile at (x, y) to
	 *        determine adjacency. Searches the 8 tiles surrounding (x, y).
	 * @note This is called by @link StitchTexture @endlink
	 *
	 * @param x the center of the area to check along the x axis
	 * @param y the center of the area to check along the y axis
	 *
	 * @returns @link byte @endlink representing adjacency to tiles with the ID
	 *          @link TileID::ground @endlink tiles encoded as bit flags
	 */
	auto MarchSquares(const uint32_t x, const uint32_t y) -> byte;
	void ParseData(const vector<char>& data);

	/**
	 * @param mask Bit mask representing adjacency.
	 *        see @link MarchSquares @endlink
	 *
	 * @returns An array of 4 rectangles used to copy sprites from
	 *          @link sprites @endlink into @link img @endlink
	 */
	static auto GetRects(const byte mask) -> array<Rectangle, 4>;
	template <typename T>
	static inline void InsertAsBytes(vector<byte>& vec, T data);

	AssetManager& am;
	std::string name;
	std::string filepath;
	Image img;
	Image sprites;
	vector<Tile> grid;
	vector<Rectangle> colliders;
	vector<Entity_ptr> entities;
	vector<Entity_ptr> spawnQueue;
	vector<IToggleable*> toggleBlocks;
	Texture tex;
	Vector2 playerStartPos{0.0f, 0.0f};
	float gravity;
	uint32_t height{0};
	uint32_t length{0};
	bool toggleState{false};
	bool toggledThisFrame{false};
	bool saved{false};
};
