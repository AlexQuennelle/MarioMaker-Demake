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
	
	if (iframetimer > 0)
	{
		iframetimer -= GetFrameTime();
	}

	CheckCollisions();
}

void Player::CheckCollisions()
{
	// no collision needed when falling off screen
	if (this->dead)
		return;

	Rectangle playerCol{GetCollisionRect()};

	for (Entity* e_ptr : level.GetEntities())
	{
		Rectangle entityCol{e_ptr->GetCollider()};
		if (CheckCollisionRecs(playerCol, entityCol))
		{
			this->level.HandleRequest(e_ptr->OnPlayerCollision(*this));
		}
	}

	if (this->dead)
		return;

	vector<Rectangle> solidCols = level.GetSolidEntityColliders();
	vector<Rectangle> levelCols = level.GetColliders();

	solidCols.reserve(solidCols.size() + levelCols.size());

	solidCols.insert(solidCols.end(), levelCols.begin(), levelCols.end());

	for (const Rectangle col : solidCols)
	{
		playerCol = GetCollisionRect();

		if (CheckCollisionRecs(playerCol, col))
		{
			RecCollisionInfo info = GetCollisionInfo(playerCol, col);

			// Adjusted object position based on minimum distance
			if (info.minDistX < info.minDistY)
			{
				this->position.x += copysignf(info.minDistX, info.delta.x);
				this->velocity.x = 0;
			}
			else
			{
				// cancel jump holding if vertical collision
				this->cancelJump = (info.delta.y > 0);

				this->position.y += copysignf(info.minDistY, info.delta.y);
				this->velocity.y = 0;
			}
		}
	}
}

Rectangle Player::GetCollisionRect()
{
	float height;
	if (this->big)
	{
		height = crouching ? 1.0f : 1.6f;
	}
	else
	{
		height = crouching ? 0.6f : 1.0f;
	}
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

	// anim update
	if (accumulatedAnimTime >= timeBetweenFrames)
	{
		accumulatedAnimTime = 0;
		curFrame++;
		showSprite = !showSprite;
	}
	if (curFrame > 2)
	{
		curFrame = 0;
	}

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

	if (this->luigi)
	{
		frameRec.y += assets.luigiOffset;
	}

	if (this->big)
	{
		frameRec.y += 64;
	}

	if (this->iframetimer <= 0 || showSprite)
	{
		DrawTextureRec(
			assets.sprites, frameRec,
			{(position.x * 16.0f) - 16.0f, (position.y * 16.0f) - 32.0f},
			WHITE);
	}

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
	this->big = false;
	this->fire = false;
	this->iframetimer = 0;
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

	vector<Rectangle> solidCols = level.GetSolidEntityColliders();
	vector<Rectangle> levelCols = level.GetColliders();

	solidCols.reserve(solidCols.size() + levelCols.size());

	solidCols.insert(solidCols.end(), levelCols.begin(), levelCols.end());

	return std::ranges::any_of(
		solidCols, //
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

void Player::TakeDamage()
{
	if (iframetimer > 0)
		return;

	if (!this->big)
	{
		Die();
		return;
	}

	iframetimer = 1;

	if (this->fire)
	{
		this->fire = false;
	}
	else
	{
		this->big = false;
	}
}