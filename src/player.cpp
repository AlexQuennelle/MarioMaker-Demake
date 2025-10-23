#include "player.h"
#include "utils.h"

Player::Player() 
	: position({0,0}), acceleration({0, 0}), velocity({0, 0}), grounded(false),
	  running(false)
{

}

void Player::Update() {

}

void Player::Draw()
{
	DrawRectangle(position.x * 16.0f, -position.y * 16.0f, 16, 16, WHITE);
}

void Player::HandleMovement(const bool running, const float input) {

}

void Player::HandleJump() {

}

void Player::Reset(const Vector2& startPosition) {
	this->position = startPosition;
}