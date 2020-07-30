#pragma once

#include <Enlivengine/Application/StateManager.hpp>

#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Core/World.hpp>
#include <Enlivengine/Core/EntityManager.hpp>
#include <Enlivengine/Core/Entity.hpp>
#include <Enlivengine/Core/ObjectEditor.hpp>
#include <Enlivengine/Core/DataFile.hpp>

#include <SFML/Graphics/Sprite.hpp>

#include "Components.hpp"

class GameState : public en::State
{
public:
	GameState(en::StateManager& manager);

	bool update(en::Time dt);

	void render(sf::RenderTarget& target);

private:
	void Init();

	void Velocity(en::Time dt);
	void PlayerMvt(en::Time dt, en::U32 index);
	void AIMvt(en::Time dt);
	void ShootUpdate(en::Time dt);
	void PlayerShoot(en::Time dt, en::U32 index);
	void Shoot(const en::Entity& entity);
	void SpawnSwarm(en::U32 swarmIndex, const en::Vector2f& pos);
	void SpawnEnemy(const en::Vector2f& pos, en::F32 rotation, en::U32 swarmIndex, const en::Color& color);
	void SpawnPlanet(const en::Vector2f& pos);

#ifdef ENLIVE_DEBUG
	void DebugUpdate(en::Time dt);
#endif // ENLIVE_DEBUG

	static constexpr en::F32 zoom = 2.0f;

	static constexpr en::U32 magicBarLines = 6;
	static constexpr en::Time magicBarTime = en::Time::Milliseconds(100);
	sf::Sprite magicBarSprite;
	en::U32 magicBarCurrent;
	en::Time magicBarAcc;
};