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
