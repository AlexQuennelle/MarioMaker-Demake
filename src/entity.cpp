#include "entity.h"
#include "assetmanager.h"

#include <raylib.h>
#include <raymath.h>

RecCollisionInfo GetCollisionInfo(Rectangle col1, Rectangle col2)
{
	// Calculation of centers of rectangles
	const Vector2 center1 = {col1.x + (col1.width / 2),
							 col1.y + (col1.height / 2)};
	const Vector2 center2 = {col2.x + (col2.width / 2),
							 col2.y + (col2.height / 2)};

	// Calculation of the distance vector between the centers of the
	// rectangles
	const Vector2 delta = Vector2Subtract(center1, center2);

	// Calculation of half-widths and half-heights of rectangles
	const Vector2 hs1 = {col1.width * .5f, col1.height * .5f};
	const Vector2 hs2 = {col2.width * .5f, col2.height * .5f};

	// Calculation of the minimum distance at which the two rectangles
	// can be separated
	const float minDistX = hs1.x + hs2.x - fabsf(delta.x);
	const float minDistY = hs1.y + hs2.y - fabsf(delta.y);

	return {.delta = delta, .minDistX = minDistX, .minDistY = minDistY};
};

Entity::Entity(const int x, const int y, AssetManager& assetManager)
	: position(x, y), assetManager(assetManager)
{}
