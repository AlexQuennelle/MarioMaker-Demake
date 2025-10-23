#pragma once

#include "utils.h"

class Player
{
	public:
	Player();
	void HandleMovement(const bool, const Vector2);
	void HandleJump();
	void Update();
	void Draw();
	void Reset(const Vector2 startPosition);

	private:
	Vector2 position;
	Vector2 velocity;
	Vector2 acceleration;
	float maxWalkSpeed;
	float maxRunSpeed;
	float baseAcceleration;
	float runAccelerationMult;
	float groundFrictionFactor;
	bool running;
	Vector2 lastInput;

	bool Grounded();
};