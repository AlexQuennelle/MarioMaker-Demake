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
	/**
	 * @brief Rebuilds the level from the tile grid. This respawns all entities.
	 */
	void Reset();

	/**
	 * @brief Sets a tile in the grid at position (x, y). If that position is
	 *        outside the bounds of the level, this method fails.
	 *
	 * @param tile ID of the tile to be set
	 * @param x the x position to set the tile
	 * @param y the y position to set the tile
	 */
	void SetTileAt(const TileID tile, const int x, const int y);
	/**
	 * @param x the x position to query
	 * @param y the y position to query
	 *
	 * @returns The @link TileID @endlink in the grid at position (x, y).
	 * @warning if (x, y) is outside the bounds of the level, this method
	 *          returns a @link TileID::ground @endlink.
	 */
	TileID TileAt(const int x, const int y);

	private:
	/**
	 * @brief Populates the colliders vector with collision rectangles. These
	 *        are generated using a 2D greedy meshing algorithm to minimize the
	 *        amount of collision checks required.
	 */
	void GenCollisionMap();
	/**
	 * @brief Generates a collision rectangle starting at (x, y) that expands as
	 *        much to the left and down as it can without encountering a cell
	 *        that has already been visited or that isn't ground.
	 * @note This is called by @link GenCollisionMap @endlink
	 *
	 * @note The visited parameter is mutated to reflect any newly visited cells
	 *
	 * @param x Starting x position for the output rectangle.
	 * @param y Starting y position for the output rectangle.
	 * @param visited A vector that mirrors @link grid @endlink storing wich
	 *        cells have been visited by the greedy meshing.
	 *
	 * @returns a @link CollisionRect @endlink
	 */
	CollisionRect GenCollisionRect(const int x, const int y,
								   vector<bool>& visited);
	/**
	 * @brief Stitches ground tile sprites into an image representing the entire
	 *        level using the data in @link grid @endlink. This is done using
	 *        marching squares algorithm to determine which sprite variant
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
	 * @returns @link byte @endlink representing adjacency to
	 *          @link TileID::ground @endlink tiles encoded as bit flags
	 */
	byte MarchSquares(const int x, const int y);
	/**
	 * @param mask Bit mask representing adjacency.
	 *        see @link MarchSquares @endlink
	 *
	 * @returns An array of 4 rectangles used to copy sprites from
	 *          @link sprites @endlink into @link img @endlink
	 */
	static array<Rectangle, 4> GetRects(const byte mask);

	int32_t height;
	int32_t length;
	Vector2 playerStart;
	vector<TileID> grid;
	vector<CollisionRect> colliders;
	Image img;
	Image sprites;
	Texture tex;
	// TODO: Vector of entity
};
