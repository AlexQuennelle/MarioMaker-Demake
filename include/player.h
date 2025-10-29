#pragma once

#include "assetmanager.h"
#include "level.h"

class Player
{
	public:
	Player(Level&, PlayerAssets);
	void HandleMovement(const bool, const Vector2);
	void HandleJump(const bool);
	void Update();
	void Draw();
	void Reset(const Vector2 startPosition);
	void AddForce(const Vector2 force);

	private:
	Vector2 position{0, 0};
	Vector2 velocity{0, 0};
	Vector2 acceleration{0, 0};
	const float maxWalkSpeed{0.1f};
	const float maxRunSpeed{0.18f};
	const float baseAcceleration{0.4f};
	const float runAccelerationMult{1.8f};
	const float groundFrictionFactor{0.92f};
	const float jumpForce{0.25f};
	const float maxTimeJumping{0.15f};
	float timeJumping{0};
	bool running{false};
	bool jumpPressed{false};
	bool cancelJump{false};
	bool canJump{true};
	bool facingRight{true};
	bool luigi{true};
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.06f};
	int curFrame{0};
	Vector2 lastInput{0, 0};
	Level& level;
	PlayerAssets assets;

	bool Grounded();
	void CheckCollisions();
	const Rectangle GetCollisionRect();
};
