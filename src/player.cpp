#include "player.h"
#include "assetmanager.h"
#include "physics.h"

#include <algorithm>
#include <raylib.h>
#include <raymath.h>

Player::Player(Level& level, PlayerAssets assets) : assets(assets), level(level)
{ }

void Player::Update()
{
	// only move if not crouching unless airborne
	if (!this->crouching || !this->Grounded())
	{
		// add base acceleration
		float horizAcceleration
			= lastInput.x * baseAcceleration * GetFrameTime();

		// apply run speed bonus if not crouching
		if (this->running && !this->crouching)
		{
			horizAcceleration *= this->runAccelerationMult;

			// less acceleration when turning
			if (this->Grounded()
				&& ((this->velocity.x > 0.0f && this->lastInput.x < 0.0f)
					|| (this->velocity.x < 0.0f && this->lastInput.x > 0.0f)))
			{
				horizAcceleration *= 0.5f;
			}

			// full sprint after charge
			// TODO: Maybe extract to local bool
			if (((this->velocity.x >= 0.15f && this->lastInput.x > 0.0f)
				 || (this->velocity.x <= -0.15f && this->lastInput.x < 0.0f))
				&& Grounded())
			{
				velocity.x = copysignf(this->maxRunSpeed, this->lastInput.x);
			}
		}
		// apply movement force to acceleration
		this->acceleration.x += horizAcceleration;
	}

	velocity = Vector2Add(velocity, acceleration);

	// constrain horizontal movement speed
	if (this->running && !this->crouching)
	{
		this->velocity.x
			= Clamp(this->velocity.x, -this->maxRunSpeed, this->maxRunSpeed);
	}
	else
	{
		this->velocity.x
			= Clamp(this->velocity.x, -this->maxWalkSpeed, this->maxWalkSpeed);
	}

	// jumping
	if (this->jumpPressed && this->canJump)
	{
		// initial launch from ground
		if (this->Grounded())
		{
			this->velocity.y = -this->jumpForce;
			this->timeJumping = 0;
			this->cancelJump = false;
		}
		// jump button held
		else if (this->timeJumping < this->maxTimeJumping && !this->cancelJump)
		{
			this->velocity.y = -this->jumpForce;
			this->timeJumping += GetFrameTime();
		}
		// cancel jump holding
		else
		{
			this->canJump = false;
		}
	}
	// prevent midair jumps
	else if (!this->Grounded())
	{
		this->canJump = false;
	}
	// prevent holding to jump multiple times
	else if (!this->jumpPressed)
	{
		this->canJump = true;
	}

	this->position = Vector2Add(this->position, this->velocity);

	if (this->Grounded())
	{
		this->crouching = this->lastInput.y < 0;

		// apply ground friction
		this->velocity.x *= this->groundFrictionFactor;
	}

	// reset acceleration
	this->acceleration = {.x = 0, .y = 0};

	if (this->iframetimer > 0)
	{
		this->iframetimer -= GetFrameTime();
	}

	for (size_t i{0}; i < this->fireballs.size(); i++)
	{
		if (!this->fireballs[i]->IsActive())
		{
			this->fireballs.erase(this->fireballs.begin()
								  + static_cast<int32_t>(i));
		}
	}

	float xClamp
		= Clamp(this->position.x, 0.0f + (this->GetCollisionRect().width / 2),
				static_cast<float>(this->level.GetLength())
					- (this->GetCollisionRect().width / 2));
	if (this->position.x != xClamp)
	{
		this->position.x = xClamp;
		this->velocity.x = 0;
	}

	if (this->position.y
		>= static_cast<float>(this->level.GetHeight())
		+ this->GetCollisionRect().height)
	{
		this->Die(false);
	}

	this->CheckCollisions();
}

void Player::CheckCollisions()
{
	// no collision needed when falling off screen
	if (this->dead)
		return;

	Rectangle playerCol{GetCollisionRect()};
	Collider newPlayerCol{GetCollisionRect()};

	for (Entity* e_ptr : this->level.GetEntities())
	{
		Rectangle entityCol{e_ptr->GetCollider()};
		Collider newEntityCol{e_ptr->GetCollider()};
		auto test{CheckColliderOverlap(newPlayerCol, newEntityCol)};
		if (CheckCollisionRecs(playerCol, entityCol))
		{
			this->level.HandleRequest(e_ptr->OnPlayerCollision(*this));
		}
	}

	if (this->dead)
		return;

	vector<Rectangle> solidCols = this->level.GetSolidEntityColliders();

	// filter entity colliders by x-distance to mario

	vector<Rectangle> levelCols = this->level.GetColliders();

	solidCols.reserve(solidCols.size() + levelCols.size());

	solidCols.insert(solidCols.end(), levelCols.begin(), levelCols.end());

	// sort solid cols by xy distance to player (start with closest one)

	for (const Rectangle col : solidCols)
	{
		playerCol = this->GetCollisionRect();

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

	for (auto& fireball : this->fireballs)
	{
		fireball->Update(this->level.GetEntities(), solidCols);
	}
}

auto Player::GetCollisionRect() -> Rectangle
{
	float height{};
	if (this->big)
	{
		height = this->crouching ? 1.0f : 1.6f;
	}
	else
	{
		height = this->crouching ? 0.6f : 1.0f;
	}
	return {
		.x = this->position.x - 0.3f,
		.y = this->position.y - height,
		.width = 0.6f,
		.height = height,
	};
}

void Player::Draw()
{
	for (auto& fireball : this->fireballs)
	{
		fireball->Draw();
	}

	// flip sprite
	float recWidth = this->facingRight ? -32 : 32;

	Rectangle frameRec{0, 0, recWidth, 32};

	// anim update
	if (this->accumulatedAnimTime >= timeBetweenFrames)
	{
		this->accumulatedAnimTime = 0;
		this->curFrame++;
		this->showSprite = !showSprite;
	}
	if (this->curFrame > 2)
	{
		this->curFrame = 0;
	}

	if (this->dead)
	{
		frameRec
			= {.x = 160.0f, .y = 32.0f, .width = recWidth, .height = 32.0f};
	}
	else if (this->crouching)
	{
		frameRec = {.x = 64.0f, .y = 0.0f, .width = recWidth, .height = 32.0f};
	}
	else if (this->Grounded())
	{
		if (this->lastInput.y
			> 0.0f
			&& (FloatEquals(this->lastInput.x, 0.0f) != 0))
		{
			// look up
			frameRec
				= {.x = 32.0f, .y = 0.0f, .width = recWidth, .height = 32.0f};
		}
		else if ((this->velocity.x > 0.0f && this->lastInput.x < 0.0f)
				 || (this->velocity.x < 0.0f && this->lastInput.x > 0.0f))
		{
			// skid
			frameRec
				= {.x = 0.0f, .y = 32.0f, .width = recWidth, .height = 32.0f};
		}
		else if (fabsf(this->velocity.x) > 0.05f)
		{
			if (fabsf(this->velocity.x) > 0.15f)
			{
				//running
				frameRec = {.x = 192.0f
								 + (static_cast<float>(this->curFrame) * 32.0f),
							.y = 0.0f,
							.width = recWidth,
							.height = 32.0f};
			}
			else
			{
				//walking
				frameRec = {.x = 96.0f
								 + (static_cast<float>(this->curFrame) * 32.0f),
							.y = 0.0f,
							.width = recWidth,
							.height = 32.0f};
			}
		}
	}
	else
	{
		if (this->velocity.y < 0.0f)
		{
			// jump up
			frameRec
				= {.x = 32.0f, .y = 32.0f, .width = recWidth, .height = 32.0f};
		}
		else
		{
			// falling
			frameRec
				= {.x = 64.0f, .y = 32.0f, .width = recWidth, .height = 32.0f};
		}
	}

	if (this->luigi)
	{
		frameRec.y += this->assets.luigiOffset;
	}

	if (this->big)
	{
		frameRec.y += 64.0f;
	}

	if (this->fire)
	{
		frameRec.x += this->assets.fireOffset;
	}

	if (this->iframetimer <= 0.0f || this->showSprite)
	{
		DrawTextureRec(this->assets.sprites, frameRec,
					   {(this->position.x * 16.0f) - 16.0f,
						(this->position.y * 16.0f) - 32.0f},
					   WHITE);
	}

	this->accumulatedAnimTime += GetFrameTime();

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

	if (!this->running && running)
	{
		this->TryFireball();
	}

	this->running = running;
	this->lastInput = input;
	if (input.x != 0.0f)
	{
		this->facingRight = this->lastInput.x > 0.0f;
	}
}

void Player::HandleJump(const bool jump) { this->jumpPressed = jump; }

void Player::Reset(const Vector2 startPosition)
{
	this->velocity = {.x = 0.0f, .y = 0.0f};
	this->position = startPosition;
	this->dead = false;
	this->big = false;
	this->fire = false;
	this->iframetimer = 0.0f;
}

auto Player::Grounded() -> bool
{
	// box cast underneath player
	Rectangle groundedBox{
		.x = this->position.x - 0.25f,
		.y = this->position.y,
		.width = 0.5f,
		.height = 0.1f,
	};

	vector<Rectangle> solidCols = this->level.GetSolidEntityColliders();
	vector<Rectangle> levelCols = this->level.GetColliders();

	solidCols.reserve(solidCols.size() + levelCols.size());

	solidCols.insert(solidCols.end(), levelCols.begin(), levelCols.end());

	return std::ranges::any_of(
		solidCols, [groundedBox](Rectangle col) -> bool
		{ return CheckCollisionRecs(groundedBox, col); });
}

void Player::AddForce(const Vector2 force)
{
	this->acceleration = Vector2Add(this->acceleration, force);
}

void Player::Die(bool jumpUp)
{
	if (this->dead)
		return;

	this->dead = true;
	this->lastInput = {.x = 0.0f, .y = 0.0f};
	this->velocity.x = 0;
	if (jumpUp)
	{
		this->velocity = {.x = 0.0f, .y = -0.3f};
	}
}

void Player::TakeDamage()
{
	if (this->iframetimer > 0.0f)
		return;

	if (!this->big)
	{
		this->Die();
		return;
	}

	this->iframetimer = 1.0f;

	if (this->fire)
	{
		this->fire = false;
	}
	else
	{
		this->big = false;
	}
}

void Player::TryFireball()
{
	if (!this->fire)
		return;

	if (this->fireballs.size() < 2)
	{
		this->fireballs.push_back(std::make_unique<Fireball>(
			this->assets.staticEntities, this->facingRight,
			Vector2{this->position.x, this->position.y - 1.0f}));
	}
}
