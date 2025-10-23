#include "player.h"
#include "utils.h"

Player::Player(Level& level) :
	// physics
	  position({0, 0}), acceleration({0, 0}), velocity({0, 0}),
	  maxWalkSpeed(0.4f), maxRunSpeed(1.2f),
	  baseAcceleration(0.5f), runAccelerationMult(2.5f), groundFrictionFactor(0.9f),
	// input
	  running(false), lastInput({0,0}),
	// references
	  level(level)
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

	position = Vector2Add(position, velocity);

	if (Grounded())
	{
		velocity.x *= groundFrictionFactor;
	}

	acceleration = {0, 0};
	
	CheckCollisions();
}

void Player::CheckCollisions() {
	for (const CollisionRect col : level.GetColliders())
	{
		Rectangle playerCol{GetCollisionRect()};

		Rectangle levelRect { 
			.x = col.position.x,
			.y = col.position.y,
			.width = col.size.x,
			.height = col.size.y
		};

		if (CheckCollisionRecs(playerCol, levelRect))
		{
			// Calculation of centers of rectangles
			const Vector2 center1 = {playerCol.x + playerCol.width / 2,
									 playerCol.y + playerCol.height / 2};
			const Vector2 center2 = {levelRect.x + levelRect.width / 2,
									 levelRect.y + levelRect.height / 2};

			// Calculation of the distance vector between the centers of the
			// rectangles
			const Vector2 delta = Vector2Subtract(center1, center2);

			// Calculation of half-widths and half-heights of rectangles
			const Vector2 hs1 = {playerCol.width * .5f, playerCol.height * .5f};
			const Vector2 hs2 = {levelRect.width * .5f, levelRect.height * .5f};

			// Calculation of the minimum distance at which the two rectangles
			// can be separated
			const float minDistX = hs1.x + hs2.x - fabsf(delta.x);
			const float minDistY = hs1.y + hs2.y - fabsf(delta.y);

			// Adjusted object position based on minimum distance
			if (minDistX < minDistY)
			{
				this->position.x += copysignf(minDistX, delta.x);
			}
			else
			{
				this->position.y += copysignf(minDistY, delta.y);
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

void Player::HandleJump() {

}

void Player::Reset(const Vector2 startPosition) {
	this->position = startPosition;
}

bool Player::Grounded() { return true; }

void Player::AddForce(const Vector2 force)
{
	acceleration = Vector2Add(acceleration, force);
}