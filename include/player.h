#pragma once

#include "utils.h"
#include "level.h"

class Player
{
	public:
	Player(Level&);
	void HandleMovement(const bool, const Vector2);
	void HandleJump();
	void Update();
	void Draw();
	void Reset(const Vector2 startPosition);
	void AddForce(const Vector2 force);

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
	Level& level;

	bool Grounded();
	void CheckCollisions();
	const Rectangle GetCollisionRect();
};