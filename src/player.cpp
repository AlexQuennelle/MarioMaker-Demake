#include "player.h"
#include "utils.h"
#include <raylib.h>

Player::Player(Level& level) : level(level)
{

}

void Player::Update()
{
	float horizAcceleration = lastInput.x * baseAcceleration * GetFrameTime();
	if (running)
	{
		horizAcceleration *= runAccelerationMult;
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
		.x = this->position.x - 0.5f, 
		.y = this->position.y - 1.0f,
		.width = 1.0f,
		.height = 1.0f
	};
}

void Player::Draw()
{
	DrawRectangle((position.x * 16.0f) - 8.0f, (position.y * 16.0f) - 16.0f, 16, 16, WHITE);
}

void Player::HandleMovement(const bool running, const Vector2 input) {
	this->running = running;
	this->lastInput = input;
}

void Player::HandleJump(const bool jump) { this->jumpPressed = jump; }

void Player::Reset(const Vector2 startPosition) {
	this->position = startPosition;
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
