#include "player.h"
#include "assetmanager.h"

#include <algorithm>
#include <raylib.h>
#include <raymath.h>

Player::Player(Level& level, PlayerAssets assets) : level(level), assets(assets)
{}

void Player::Update()
{
	// only move if not crouching unless airborne
	if (!crouching || !Grounded())
	{
		// add base acceleration
		float horizAcceleration =
			lastInput.x * baseAcceleration * GetFrameTime();

		// apply run speed bonus if not crouching
		if (running && !crouching)
		{
			horizAcceleration *= runAccelerationMult;

			// less acceleration when turning
			if (Grounded() && ((velocity.x > 0 && lastInput.x < 0) ||
							   (velocity.x < 0 && lastInput.x > 0)))
			{
				horizAcceleration *= 0.5f;
			}

			// full sprint after charge
			if (Grounded() && ((velocity.x >= 0.15f && lastInput.x > 0) ||
							   (velocity.x <= -0.15f && lastInput.x < 0)))
			{
				velocity.x = copysignf(maxRunSpeed, lastInput.x);
			}
		}
		// apply movement force to acceleration
		acceleration.x += horizAcceleration;
	}

	velocity = Vector2Add(velocity, acceleration);

	// constrain horizontal movement speed
	if (running && !crouching)
	{
		velocity.x = Clamp(velocity.x, -maxRunSpeed, maxRunSpeed);
	}
	else
	{
		velocity.x = Clamp(velocity.x, -maxWalkSpeed, maxWalkSpeed);
	}

	// jumping
	if (jumpPressed && canJump)
	{
		// initial launch from ground
		if (Grounded())
		{
			velocity.y = -jumpForce;
			timeJumping = 0;
			cancelJump = false;
		}
		// jump button held
		else if (timeJumping < maxTimeJumping && !cancelJump)
		{
			velocity.y = -jumpForce;
			timeJumping += GetFrameTime();
		}
		// cancel jump holding
		else
		{
			canJump = false;
		}
	}
	// prevent midair jumps
	else if (!Grounded())
	{
		canJump = false;
	}
	// prevent holding to jump multiple times
	else if (!jumpPressed)
	{
		canJump = true;
	}

	position = Vector2Add(position, velocity);

	if (Grounded())
	{
		crouching = lastInput.y < 0;

		// apply ground friction
		velocity.x *= groundFrictionFactor;
	}

	// reset acceleration
	acceleration = {.x = 0, .y = 0};

	CheckCollisions();
}

void Player::CheckCollisions()
{
	// no collision needed when falling off screen
	if (this->dead)
		return;
	for (const Rectangle col : level.GetColliders())
	{
		Rectangle playerCol{GetCollisionRect()};

		if (CheckCollisionRecs(playerCol, col))
		{
			// Calculation of centers of rectangles
			const Vector2 center1 = {playerCol.x + (playerCol.width / 2),
									 playerCol.y + (playerCol.height / 2)};
			const Vector2 center2 = {col.x + (col.width / 2),
									 col.y + (col.height / 2)};

			// Calculation of the distance vector between the centers of the
			// rectangles
			const Vector2 delta = Vector2Subtract(center1, center2);

			// Calculation of half-widths and half-heights of rectangles
			const Vector2 hs1 = {playerCol.width * .5f, playerCol.height * .5f};
			const Vector2 hs2 = {col.width * .5f, col.height * .5f};

			// Calculation of the minimum distance at which the two rectangles
			// can be separated
			const float minDistX = hs1.x + hs2.x - fabsf(delta.x);
			const float minDistY = hs1.y + hs2.y - fabsf(delta.y);

			// Adjusted object position based on minimum distance
			if (minDistX < minDistY)
			{
				this->position.x += copysignf(minDistX, delta.x);
				this->velocity.x = 0;
			}
			else
			{
				// cancel jump holding if vertical collision
				this->cancelJump = (delta.y < 0);

				this->position.y += copysignf(minDistY, delta.y);
				this->velocity.y = 0;
			}
		}
	}
}

Rectangle Player::GetCollisionRect()
{
	// THIS ASSUMES SMALL PLAYER
	float height = crouching ? 0.6f : 1.0f;
	return {.x = this->position.x - 0.3f,
			.y = this->position.y - height,
			.width = 0.6f,
			.height = height};
}

void Player::Draw()
{
	// flip sprite
	float recWidth = facingRight ? -32 : 32;

	Rectangle frameRec{0, 0, recWidth, 32};

	if (dead)
	{
		//dead
		frameRec = {.x = 160, .y = 32, .width = recWidth, .height = 32};
	}
	else if (crouching)
	{
		//crouching
		frameRec = {.x = 64, .y = 0, .width = recWidth, .height = 32};
	}
	else if (Grounded())
	{
		if (lastInput.y > 0 && (FloatEquals(lastInput.x, 0) != 0))
		{
			// look up
			frameRec = {.x = 32, .y = 0, .width = recWidth, .height = 32};
		}
		else if ((velocity.x > 0 && lastInput.x < 0) ||
				 (velocity.x < 0 && lastInput.x > 0))
		{
			// skid
			frameRec = {.x = 0, .y = 32, .width = recWidth, .height = 32};
		}
		else if (fabsf(velocity.x) > 0.05f)
		{
			// anim update
			if (accumulatedAnimTime >= timeBetweenFrames)
			{
				accumulatedAnimTime = 0;
				curFrame++;
			}
			if (curFrame > 2)
			{
				curFrame = 0;
			}

			if (fabsf(velocity.x) > 0.15f)
			{
				//running
				frameRec = {.x = 192.0f + (curFrame * 32),
							.y = 0,
							.width = recWidth,
							.height = 32};
			}
			else
			{
				//walking
				frameRec = {.x = 96.0f + (curFrame * 32),
							.y = 0,
							.width = recWidth,
							.height = 32};
			}
		}
	}
	else
	{
		if (velocity.y < 0)
		{
			// jump up
			frameRec = {.x = 32, .y = 32, .width = recWidth, .height = 32};
		}
		else
		{
			// falling
			frameRec = {.x = 64, .y = 32, .width = recWidth, .height = 32};
		}
	}

	if (luigi)
	{
		frameRec.y += assets.luigiOffset;
	}

	DrawTextureRec(assets.sprites, frameRec,
				   {(position.x * 16.0f) - 16.0f, (position.y * 16.0f) - 32.0f},
				   WHITE);

	accumulatedAnimTime += GetFrameTime();

#ifdef DRAW_COLS
	Rectangle rec = this->GetCollisionRect();
	DrawRectangleLinesEx(
		{rec.x * 16, rec.y * 16, rec.width * 16, rec.height * 16}, 1.0f,
		{0, 255, 0, 255});
#endif // DRAW_COLS
}

void Player::HandleMovement(const bool running, const Vector2 input)
{
	// block input if dead
	if (this->dead)
		return;

	this->running = running;
	this->lastInput = input;
	if (input.x != 0)
	{
		this->facingRight = lastInput.x > 0;
	}
}

void Player::HandleJump(const bool jump) { this->jumpPressed = jump; }

void Player::Reset(const Vector2 startPosition)
{
	this->position = startPosition;
	this->dead = false;
}

bool Player::Grounded()
{
	// box cast underneath player
	Rectangle groundedBox{
		.x = this->position.x - 0.25f,
		.y = this->position.y,
		.width = 0.5f,
		.height = 0.1f,
	};

	return std::ranges::any_of(
		this->level.GetColliders(), //
		[groundedBox](Rectangle col)
		{ return CheckCollisionRecs(groundedBox, col); } //
	);

	//for (const Rectangle col : level.GetColliders())
	//{
	//	if (CheckCollisionRecs(col, groundedBox))
	//	{
	//		return true;
	//	}
	//}
	//return false;
}

// Public method for applying forces to the player
void Player::AddForce(const Vector2 force)
{
	acceleration = Vector2Add(acceleration, force);
}

void Player::TemporaryDeathTest() { this->Die(); }

void Player::Die()
{
	if (this->dead)
		return;

	this->dead = true;
	this->lastInput = {.x = 0, .y = 0};
	this->velocity = {.x = 0, .y = -0.3f};
}
