#pragma once

#include "assetmanager.h"

#include <memory>
#include <raylib.h>
#include <raymath.h>
#include <variant>
#include <vector>

#ifndef NDEBUG
//#define DRAW_COLS
//#define LOG_LEVEL_DATA
#ifdef LOG_LEVEL_DATA
#include <format>
#endif // LOG_LEVEL_DATA
#endif // !NDEBUG

using std::vector;

class Player;

/** Object representing a collision between 2 @link Rectangle @endlink
 */
struct RecCollisionInfo
{
	public:
	Vector2 delta;
	float minDistX;
	float minDistY;
};

// TODO: Add documentation comment
auto GetCollisionInfo(Rectangle, Rectangle) -> RecCollisionInfo;

class Entity;
struct SpawnEntityRequest
{
	std::unique_ptr<Entity> entity;
};
struct ToggleRequest
{ };
/** @brief Request from an @link Entity @endlink to the @link Game @endlink
 *
 *  @note This is an alias for @link std::variant @endlink, and empty requests
 *        are represented by @link std::monostate @endlink
 */
using EntityReq
	= std::variant<std::monostate, SpawnEntityRequest, ToggleRequest>;

/** @brief Base entity class. This class is abstract and should never be
 *         instantiated on its own.
 */
class Entity // NOLINT
{
	public:
	Entity(const int x, const int y, AssetManager& assetManager);
	virtual ~Entity() = default;

	/** @brief The entity's main update loop. This method performs all
	 *         non-drawing related logic, such as collision and movement.
	 *
	 *  @returns A @link EntityReq @endlink containing information about actions
	 *           the entity wants to happen in the @link Game @endlink
	 */
	virtual auto Update(const vector<Rectangle>& colliders) -> EntityReq = 0;
	/**
	 * @brief The entity's main draw logic.
	 */
	virtual void Draw() = 0;
	/** @brief Special Draw method to be called in edit mode. For most entities
	 *         this should just call @link Entity::Draw() @endlink, but for
	 *         certain specific cases this method can display extra information.
	 */
	virtual void EditDraw() = 0;
	/** @brief This method should be called when a collision has been detected
	 *         between this entity and the @link player @endlink
	 *
	 *  @param player The @link Player @endlink object that the collision was
	 *         detected with
	 *
	 *  @returns A @link EntityReq @endlink containing information about actions
	 *           the entity wants to happen in the @link Game @endlink
	 */
	virtual auto OnPlayerCollision(Player& player) -> EntityReq = 0;
	/** @brief This method should be called when a collision has been detected
	 *         between this entity and another.
	 *
	 *  @param entity The @link Entity @endlink object that the collision was
	 *         detected with
	 *
	 *  @returns A @link EntityReq @endlink containing information about actions
	 *           the entity wants to happen in the @link Game @endlink
	 */
	virtual auto OnEntityCollision(Entity& entity) -> EntityReq = 0;

	virtual auto TakeDamage() -> bool { return false; };

	auto IsSolid() const -> bool { return this->solid; }
	/** Should this entity be considered an active part of the game
	 */
	auto IsActive() const -> bool { return this->isActive; }
	/** @returns A @link Rectangle @endlink representing the entity's collision
	 *           box
	 */
	auto GetCollider() const -> Rectangle
	{
		return {
			.x = this->collider.x + this->position.x,
			.y = this->collider.y + this->position.y,
			.width = this->collider.width,
			.height = this->collider.height,
		};
	};

	protected:
	Rectangle collider{0.0f, 0.0f, 1.0f, 1.0f};
	AssetManager& assetManager;
	Vector2 position;
	bool isActive{true};
	bool solid{false};
};

class Block : public Entity
{
	public:
	Block(const int x, const int y, AssetManager& assetManager,
		  const bool variant = false);

	/** @copydoc Entity::Update() */
	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	/** @copydoc Entity::Draw() */
	void Draw() override;
	/** @copydoc Entity::EditDraw() */
	void EditDraw() override;
	/** @copydoc Entity::OnPlayerCollision() */
	auto OnPlayerCollision(Player& player) -> EntityReq override;
	/** @copydoc Entity::OnEntityCollision() */
	auto OnEntityCollision(Entity& entity) -> EntityReq override;

	private:
	float bonkTimer{0.0f};
	static constexpr float totalBonkTime{0.15f};
	bool isVariant;
	Texture2D& sprite;
};

class Spike : public Entity
{
	public:
	Spike(const int x, const int y, AssetManager& assetManager);

	/** @copydoc Entity::Update() */
	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	/** @copydoc Entity::Draw() */
	void Draw() override;
	/** @copydoc Entity::EditDraw() */
	void EditDraw() override;
	/** @copydoc Entity::OnPlayerCollision() */
	auto OnPlayerCollision(Player& player) -> EntityReq override;
	/** @copydoc Entity::OnEntityCollision() */
	auto OnEntityCollision(Entity& entity) -> EntityReq override;

	private:
	Texture2D& sprite;
};

class ItemBox : public Entity
{
	public:
	ItemBox(const int x, const int y, AssetManager& assetManager,
			const bool isBrick, const bool isHidden);

	/** @copydoc Entity::Update() */
	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	/** @copydoc Entity::Draw() */
	void Draw() override;
	/** @copydoc Entity::EditDraw() */
	void EditDraw() override;
	/** @copydoc Entity::OnPlayerCollision() */
	auto OnPlayerCollision(Player& player) -> EntityReq override;
	/** @copydoc Entity::OnEntityCollision() */
	auto OnEntityCollision(Entity& entity) -> EntityReq override;

	private:
	float accumulatedAnimTime{0.0f};
	float timeBetweenFrames{0.12f};
	float bonkTimer{0.0f};
	static constexpr float totalBonkTime{0.15f};
	int curFrame{0};
	Texture2D& sprite;
	const bool isBrick;
	const bool isHidden;
	bool empty{false};
};

class Coin : public Entity
{
	public:
	Coin(const int x, const int y, AssetManager& assetmanager);

	/** @copydoc Entity::Update() */
	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	/** @copydoc Entity::Draw() */
	void Draw() override;
	/** @copydoc Entity::EditDraw() */
	void EditDraw() override;
	/** @copydoc Entity::OnPlayerCollision() */
	auto OnPlayerCollision(Player& player) -> EntityReq override;
	/** @copydoc Entity::OnEntityCollision() */
	auto OnEntityCollision(Entity& entity) -> EntityReq override;

	private:
	Texture2D& sprite;
	float accumulatedAnimTime{0};
	float timeBetweenFrames{0.12f};
	int curFrame{0};
};

class IToggleable // NOLINT
{
	public:
	virtual ~IToggleable() = default;

	/**
	 * @brief Sets the current state of the block. This should match some
	 *        'global' state stored in @link Game @endlink to keep all toggle
	 *        blocks synchronized
	 */
	virtual void SetState(const bool newState) = 0;
};
class ToggleSwitch : public Entity, public IToggleable
{
	public:
	ToggleSwitch(const int x, const int y, AssetManager& assetManager);

	/** @copydoc Entity::Update() */
	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	/** @copydoc Entity::Draw() */
	void Draw() override;
	/** @copydoc Entity::EditDraw() */
	void EditDraw() override;
	/** @copydoc Entity::OnPlayerCollision() */
	auto OnPlayerCollision(Player& player) -> EntityReq override;
	/** @copydoc Entity::OnEntityCollision() */
	auto OnEntityCollision(Entity& entity) -> EntityReq override;

	/** @copydoc IToggleable::SetState() */
	void SetState(const bool newState) override { this->on = newState; };

	private:
	float bonkTimer{0.0f};
	static constexpr float totalBonkTime{0.15f};
	bool on{false};
	Texture2D& sprite;
};
class ToggleBlock : public Entity, public IToggleable
{
	public:
	ToggleBlock(const int x, const int y, AssetManager& assetManager,
				const bool startOn);

	/** @copydoc Entity::Update() */
	auto Update(const vector<Rectangle>& colliders) -> EntityReq override;
	/** @copydoc Entity::Draw() */
	void Draw() override;
	/** @copydoc Entity::EditDraw() */
	void EditDraw() override;
	/** @copydoc Entity::OnPlayerCollision() */
	auto OnPlayerCollision(Player& /*player*/) -> EntityReq override;
	/** @copydoc Entity::OnEntityCollision() */
	auto OnEntityCollision(Entity& /*entity*/) -> EntityReq override;

	/** @copydoc IToggleable::SetState() */
	void SetState(const bool newState) override { this->on = newState; };

	private:
	const bool startOn;
	bool on;
	Texture2D& sprite;
};
