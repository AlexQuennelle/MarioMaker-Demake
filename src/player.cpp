#include "player.h"
#include "utils.h"
#include "assetmanager.h"
#include <raylib.h>

Player::Player(Level& level, PlayerAssets assets) : level(level), assets(assets) {

}

void Player::Update()
{
	float horizAcceleration = lastInput.x * baseAcceleration * GetFrameTime();
	if (running)
	{
		horizAcceleration *= runAccelerationMult;

		if (Grounded() && ((velocity.x > 0 && lastInput.x < 0) ||
			(velocity.x < 0 && lastInput.x > 0)))
		{
			horizAcceleration *= 0.5f;
		}

		// full sprint after charge
		if (Grounded() && 
			((velocity.x >= 0.15f && lastInput.x > 0) ||
			(velocity.x <= -0.15f && lastInput.x < 0)))
		{
			velocity.x = copysignf(maxRunSpeed, lastInput.x);
		}
	}
	acceleration.x += horizAcceleration;

	velocity = Vector2Add(velocity, acceleration);

	if (running)
	{
		velocity.x = Clamp(velocity.x, -maxRunSpeed, maxRunSpeed);
	}
	else
	{
		velocity.x = Clamp(velocity.x, -maxWalkSpeed, maxWalkSpeed);
	}

	if (jumpPressed && canJump)
	{
		if (Grounded())
		{
			velocity.y = -jumpForce;
			timeJumping = 0;
			cancelJump = false;
		}
		else if (timeJumping < maxTimeJumping && !cancelJump)
		{
			velocity.y = -jumpForce;
			timeJumping += GetFrameTime();
		}
		else
		{
			canJump = false;
		}
	}
	else if (!Grounded())
	{
		canJump = false;
	}
	else if (!jumpPressed)
	{
		canJump = true;
	}

	position = Vector2Add(position, velocity);

	if (Grounded())
	{
		velocity.x *= groundFrictionFactor;
	}

	acceleration = {0, 0};
	
	CheckCollisions();
}

void Player::CheckCollisions() {
	// no collision needed when falling off screen
	if (this->dead)
		return;

	for (const Rectangle col : level.GetColliders())
	{
		Rectangle playerCol{GetCollisionRect()};

		if (CheckCollisionRecs(playerCol, col))
		{
			// Calculation of centers of rectangles
			const Vector2 center1 = {playerCol.x + playerCol.width / 2,
									 playerCol.y + playerCol.height / 2};
			const Vector2 center2 = {col.x + col.width / 2,
									 col.y + col.height / 2};

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
				this->cancelJump = (delta.y < 0);
				this->position.y += copysignf(minDistY, delta.y);
				this->velocity.y = 0;
			}
		}
	}
}

const Rectangle Player::GetCollisionRect() {
	// THIS ASSUMES SMALL PLAYER
	return
	{
		.x = this->position.x - 0.3f, 
		.y = this->position.y - 1.0f,
		.width = 0.6f,
		.height = 1.0f
	};
}

void Player::Draw()
{
	float recWidth = facingRight ? -32 : 32;

	Rectangle frameRec{0, 0, recWidth, 32};

	if (dead)
	{
		//dead
		frameRec = {160, 32, 32, 32};
	}
	else if (Grounded())
	{
		if (lastInput.y > 0 && FloatEquals(lastInput.x, 0))
		{
			// look up
			frameRec = {32, 0, recWidth, 32};
		}
		else if ((velocity.x > 0 && lastInput.x < 0) ||
				 (velocity.x < 0 && lastInput.x > 0))
		{
			// skid
			frameRec = {0, 32, recWidth, 32};
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
				frameRec = {192.0f + (curFrame * 32), 0, recWidth, 32};
			}
			else
			{
				//walking
				frameRec = {96.0f + (curFrame * 32), 0, recWidth, 32};
			}
		}
	}
	else
	{
		if (velocity.y < 0)
		{
			// jump up
			frameRec = {32, 32, recWidth, 32};
		}
		else
		{
			// falling
			frameRec = {64, 32, recWidth, 32};
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
	DrawRectangleLinesEx({rec.x * 16, rec.y * 16,
							 rec.width * 16, rec.height * 16},
							 1.0f, {0, 255, 0, 255});
	#endif // DRAW_COLS
}

void Player::HandleMovement(const bool running, const Vector2 input) {
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

void Player::Reset(const Vector2 startPosition) {
	this->position = startPosition;
	this->dead = false;
}

bool Player::Grounded() 
{ 
	Rectangle groundedBox
	{
		.x = this->position.x - 0.5f, 
		.y = this->position.y, 
		.width = 1.0f,
		.height = 0.1f
	};

	for (const Rectangle col : level.GetColliders())
	{
		if (CheckCollisionRecs(col, groundedBox))
		{
			return true;
		}	
	}
	return false;
}

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
	this->lastInput = {0, 0};
	this->velocity = {0, -0.3f};
}