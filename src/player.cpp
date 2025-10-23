#include "player.h"
#include "utils.h"

Player::Player() :
	// physics
	  position({0, 0}), acceleration({0, 0}), velocity({0, 0}),
	  maxWalkSpeed(0.4f), maxRunSpeed(1.2f),
	  baseAcceleration(0.5f), runAccelerationMult(2.5f), groundFrictionFactor(0.9f),
	// input
	  running(false), lastInput({0,0})
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
	
}

void Player::Draw()
{
	DrawRectangle(position.x * 16.0f, -position.y * 16.0f, 16, 16, WHITE);
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