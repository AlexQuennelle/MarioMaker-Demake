#pragma once

#include <raylib.h>
#include <vector>

class Collider
{
	public:
	Collider(const Rectangle rect);

	private:
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
};

struct HitInfo
{
	public:
	Vector2 hitNormal{.x = 0.0f, .y = 0.0f};
	float hitDepth{0.0f};
	bool isHit{false};
};

auto CheckColliderOverlap(const Collider& col1, const Collider& col2)
	-> HitInfo;
