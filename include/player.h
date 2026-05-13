#pragma once

#include "assetmanager.h"
#include "fireball.h"
#include "level.h"

class Player
{
	public:
	Player(Level&, PlayerAssets);

	void Update();
	void Draw();

	void HandleMovement(const bool, const Vector2);
	void HandleJump(const bool);
	void AddForce(const Vector2 force);
	void CancelJump() { this->cancelJump = true; }

	void TakeDamage();
	void Die(bool jumpUp = true);
	void GainCoin() { this->coins++; }
	void GetBig() { this->big = true; }
	void GetFire()
	{
		this->big = true;
		this->fire = true;
	}

	void Reset(const Vector2 startPosition);

	//getters
	auto IsDead() const -> bool { return this->dead; }
	auto IsBig() const -> bool { return this->big; }
	auto IsJumpPressed() const -> bool { return this->jumpPressed; }
	auto GetCoins() const -> int { return this->coins; }
	auto GetPosition() const -> Vector2 { return this->position; }
	auto GetVelocity() const -> Vector2 { return this->velocity; }
	auto GetCollisionRect() -> Rectangle;

	// EVIL SETTER AAAAAAA (i kinda need it)
	void SetVelocity(const Vector2 velocity) { this->velocity = velocity; }

	private:
	auto Grounded() -> bool;
	void CheckCollisions();
	void TryFireball();

	PlayerAssets assets;
	Level& level;
	std::vector<std::unique_ptr<Fireball>> fireballs;
	Vector2 position{.x = 0, .y = 0};
	Vector2 velocity{.x = 0, .y = 0};
	Vector2 acceleration{.x = 0, .y = 0};
	Vector2 lastInput{.x = 0, .y = 0};
	const float maxWalkSpeed{0.1f};
	const float maxRunSpeed{0.17f};
	const float baseAcceleration{0.4f};
	const float runAccelerationMult{2.0f};
	const float groundFrictionFactor{0.94f};
	const float jumpForce{0.25f};
	const float maxTimeJumping{0.15f};
	float timeJumping{0};
	float iframetimer{0};
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.06f};
	int curFrame{0};
	int coins{0};
	bool running{false};
	bool jumpPressed{false};
	bool crouching{false};
	bool cancelJump{false};
	bool canJump{true};
	bool facingRight{true};
	bool luigi{false};
	bool dead{false};
	bool big{false};
	bool fire{false};
	bool star{false};
	bool showSprite{true};
};
