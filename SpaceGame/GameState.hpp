#pragma once

#include <Enlivengine/Application/StateManager.hpp>

#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Core/World.hpp>
#include <Enlivengine/Core/EntityManager.hpp>
#include <Enlivengine/Core/Entity.hpp>
#include <Enlivengine/Core/ObjectEditor.hpp>
#include <Enlivengine/Core/DataFile.hpp>

#include "Components.hpp"

class GameState : public en::State
{
public:
	GameState(en::StateManager& manager);
	~GameState();

	bool handleEvent(const sf::Event& event);

	bool update(en::Time dt);

	void render(sf::RenderTarget& target);

private:
	void Init();

#ifdef ENLIVE_DEBUG
	void DebugEvent(const sf::Event& event);
	void DebugUpdate(en::Time dt);
#endif // ENLIVE_DEBUG

private:
	en::World mWorld;
};