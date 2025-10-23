#pragma once

#include "utils.h"

class Player
{
	public:
	Player();
	void HandleMovement(const bool, const float);
	void HandleJump();
	void Update();
	void Draw();
	void Reset(const Vector2& startPosition);

	private:
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	bool grounded;
	bool running;
};