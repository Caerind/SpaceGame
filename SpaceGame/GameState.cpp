#include "GameState.hpp"

#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Core/Universe.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <Enlivengine/Tools/ImGuiEntityBrowser.hpp>

#include <Enlivengine/Core/Components.hpp>
#include "Components.hpp"

#include "GameSingleton.hpp"
#include "AudioTrackMixer3000.hpp"

GameState::GameState(en::StateManager& manager)
	: en::State(manager)
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

	const sf::View previousView = target.getView();

	auto& world = GameSingleton::GetInstance().world;

#ifdef ENLIVE_DEBUG
	bool editor = true;
	if (editor)
	{
		target.setView(world.GetFreeCamView().getHandle());
	}
	else
	{
		target.setView(world.GetGameView().getHandle());
	}
#else
	target.setView(world.GetGameView().getHandle());
#endif // ENLIVE_DEBUG

#ifdef ENLIVE_DEBUG
	static bool dotInit = false;
	static sf::CircleShape dotTransform;
	if (!dotInit)
	{
		static constexpr float radius = 2.0f;
		dotTransform.setFillColor(sf::Color::Red);
		dotTransform.setRadius(radius);
		dotTransform.setOrigin(sf::Vector2f(radius, radius));
		dotInit = true;
	}
#endif // ENLIVE_DEBUG

	{
		ENLIVE_PROFILE_SCOPE(RenderSort);
		world.GetEntityManager().GetRegistry().sort<en::TransformComponent>([](const auto& lhs, const auto& rhs)
		{
			const auto& pL = lhs.transform.GetPosition();
			const auto& pR = rhs.transform.GetPosition();
			if (pL.z == pR.z)
			{
				return pL.y < pR.y;
			}
			return pL.z < pR.z;
		});
	}

	{
		ENLIVE_PROFILE_SCOPE(RenderSystem);
		auto transformView = world.GetEntityManager().View<en::RenderableComponent>();
		for (auto entt : transformView)
		{
			en::Entity entity(world.GetEntityManager(), entt);
			if (entity.IsValid())
			{
				sf::RenderStates states;
				if (entity.Has<en::TransformComponent>())
				{
					states.transform = en::toSF(entity.Get<en::TransformComponent>().transform.GetMatrix());
				}

				if (entity.Has<en::SpriteComponent>())
				{
					entity.Get<en::SpriteComponent>().sprite.Render(target, states);
				}
				if (entity.Has<en::TextComponent>())
				{
					entity.Get<en::TextComponent>().text.Render(target, states);
				}
				if (entity.Has<ShipComponent>())
				{
					entity.Get<ShipComponent>().Render(target, states);
				}

#ifdef ENLIVE_DEBUG
				if (en::ImGuiEntityBrowser::GetInstance().IsSelected(entity))
				{
					target.draw(dotTransform, states);
				}
#endif // ENLIVE_DEBUG
			}
		}
	}

	target.setView(previousView);
}

void GameState::Init()
{
	auto& world = GameSingleton::GetInstance().world;

	en::DataFile file;
	file.LoadFromFile(en::PathManager::GetInstance().GetAssetsPath() + "world.xml");
	file.Deserialize(world, "World");

#ifdef ENLIVE_DEBUG
	world.GetFreeCamView().setCenter(getApplication().GetWindow().getMainView().getSize() * 0.5f);
	world.GetFreeCamView().setSize(getApplication().GetWindow().getMainView().getSize());
#endif // ENLIVE_DEBUG

	en::Universe::GetInstance().SetCurrentWorld(&world);

	auto player1 = world.GetEntityManager().CreateEntity();
	player1.Add<en::NameComponent>().name = "Player1";
	player1.Add<en::TransformComponent>();
	player1.Add<en::RenderableComponent>();
	player1.Add<VelocityComponent>();
	player1.Add<ShipComponent>();
	GameSingleton::GetInstance().player1 = player1;

	auto player2 = world.GetEntityManager().CreateEntity();
	player2.Add<en::NameComponent>().name = "Player2";
	player2.Add<en::TransformComponent>();
	player2.Add<en::RenderableComponent>();
	player2.Add<VelocityComponent>();
	player2.Add<ShipComponent>();
	GameSingleton::GetInstance().player2 = player2;
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
			fileWorld.Serialize(GameSingleton::GetInstance().world, "World");
			fileWorld.SaveToFile(en::PathManager::GetInstance().GetAssetsPath() + "world.xml");

			en::DataFile fileGame;
			fileGame.CreateEmptyFile();
			fileGame.Serialize(GameSingleton::GetInstance(), "Game");
			fileGame.Serialize(AudioTrackMixer3000::GetInstance(), "Audio");
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
		GameSingleton::GetInstance().world.GetFreeCamView().move(velocity);
	}
}
#endif // ENLIVE_DEBUG