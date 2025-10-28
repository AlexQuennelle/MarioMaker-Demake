#pragma once

#include "level.h"
#include "assetmanager.h"

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
	Vector2 position{0,0};
	Vector2 velocity{0,0};
	Vector2 acceleration{0,0};
	const float maxWalkSpeed{0.1f};
	const float maxRunSpeed{0.2f};
	const float baseAcceleration{0.5f};
	const float runAccelerationMult{2.5f};
	const float groundFrictionFactor{0.9f};
	const float jumpForce{0.3f};
	const float maxTimeJumping{0.15f};
	float timeJumping{0};
	bool running{false};
	bool jumpPressed{false};
	bool cancelJump{false};
	bool canJump{true};
	Vector2 lastInput{0,0};
	Level& level;
	PlayerAssets assets;

	bool Grounded();
	void CheckCollisions();
	const Rectangle GetCollisionRect();
};
