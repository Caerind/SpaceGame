#include "GameState.hpp"

#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Core/Universe.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "GameSingleton.hpp"

GameState::GameState(en::StateManager& manager)
	: en::State(manager)
	, mWorld()
{
	Init();
}

GameState::~GameState()
{
}

bool GameState::handleEvent(const sf::Event& event)
{
	ENLIVE_PROFILE_FUNCTION();

#ifdef ENLIVE_DEBUG
	DebugEvent(event);
#endif // ENLIVE_DEBUG

	return false;
}

bool GameState::update(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

	if (!en::Application::GetInstance().IsRunning())
	{
		return false;
	}

#ifdef ENLIVE_DEBUG
	DebugUpdate(dt);
#endif // ENLIVE_DEBUG

	return false;
}

void GameState::render(sf::RenderTarget& target)
{
	ENLIVE_PROFILE_FUNCTION();
	mWorld.Render(target);
}

void GameState::Init()
{
	en::DataFile file;
	file.LoadFromFile(en::PathManager::GetInstance().GetAssetsPath() + "world.xml");
	file.Deserialize(mWorld, "World");

#ifdef ENLIVE_DEBUG
	mWorld.GetFreeCamView().setCenter(getApplication().GetWindow().getMainView().getSize() * 0.5f);
	mWorld.GetFreeCamView().setSize(getApplication().GetWindow().getMainView().getSize());
#endif // ENLIVE_DEBUG

	en::Universe::GetInstance().SetCurrentWorld(&mWorld);

	auto& actionSystem = getApplication().GetActionSystem();
	actionSystem.AddInputJoystickConnect("player1JoystickConnected", 0, en::ActionType::Hold);
	actionSystem.AddInputJoystickConnect("player2JoystickConnected", 1, en::ActionType::Hold);
	actionSystem.AddInputJoystickConnect("player1JoystickConnect", 0, en::ActionType::Pressed);
	actionSystem.AddInputJoystickConnect("player2JoystickConnect", 1, en::ActionType::Pressed);
	actionSystem.AddInputJoystickConnect("player1JoystickDisconnect", 0, en::ActionType::Released);
	actionSystem.AddInputJoystickConnect("player2JoystickDisconnect", 1, en::ActionType::Released);
	actionSystem.AddInputJoystickButton("player1JoystickButtonA", 0, 0, en::ActionType::Pressed);
	actionSystem.AddInputJoystickButton("player2JoystickButtonA", 1, 0, en::ActionType::Pressed);



}

#ifdef ENLIVE_DEBUG
void GameState::DebugEvent(const sf::Event& event)
{
	ENLIVE_PROFILE_FUNCTION();

	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.control == true && event.key.code == sf::Keyboard::S)
		{
			en::DataFile fileWorld;
			fileWorld.CreateEmptyFile();
			fileWorld.Serialize(mWorld, "World");
			fileWorld.SaveToFile(en::PathManager::GetInstance().GetAssetsPath() + "world.xml");

			en::DataFile fileGame;
			fileGame.CreateEmptyFile();
			fileGame.Serialize(GameSingleton::GetInstance(), "Game");
			fileGame.SaveToFile(en::PathManager::GetInstance().GetAssetsPath() + "game.xml");

			enLogInfo(en::LogChannel::Global, "Saved");
		}
	}
}

void GameState::DebugUpdate(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

	// TODO : Move to Engine ?
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
	{
		constexpr en::F32 speed = 400.0f;
		en::F32 speedMulti = (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) ? 2.0f : 1.0f;
		en::F32 speedDiv = (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)) ? 0.5f : 1.0f;
		en::Vector2f velocity(en::Vector2f::zero);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			velocity.y -= 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			velocity.y += 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			velocity.x -= 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			velocity.x += 1.0f;
		}
		if (velocity.getSquaredLength() > 0.01f)
		{
			velocity.normalize();
		}
		velocity *= speed * speedMulti * speedDiv * dt.AsSeconds();
		mWorld.GetFreeCamView().move(velocity);
	}
}
#endif // ENLIVE_DEBUG