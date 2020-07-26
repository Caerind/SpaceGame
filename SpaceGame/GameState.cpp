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

	GameSingleton::GetInstance().UpdateStats(dt);

	Velocity(dt);

	// MagicBar
	magicBarAcc += dt;
	if (magicBarAcc >= magicBarTime)
	{
		magicBarCurrent++;
		if (magicBarCurrent >= magicBarLines)
		{
			magicBarCurrent = 0;
		}
		magicBarAcc -= magicBarTime;
		magicBarSprite.setTextureRect(sf::IntRect(0, static_cast<int>(magicBarCurrent) * 64, 1024, 64));
	}

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

	// Background
	static bool backgroundInitialized = false;
	static sf::Sprite background;
	if (!backgroundInitialized)
	{
		en::Texture& texture = en::ResourceManager::GetInstance().Get<en::Texture>("background").Get();
		texture.setRepeated(true);
		background.setTexture(texture);
		background.setTextureRect(sf::IntRect(0, 0, 2048 * 6, 2048 * 6));
		backgroundInitialized = true;
	}
	en::I32 x = static_cast<en::I32>(GameSingleton::GetInstance().posCenter.x) / 2048;
	en::I32 y = static_cast<en::I32>(GameSingleton::GetInstance().posCenter.y) / 2048;
	background.setPosition((x-2) * 2048, (y-2) * 2048);
	target.draw(background);

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

		const auto pPlayer1 = GameSingleton::GetInstance().player1.Get<en::TransformComponent>().transform.GetPosition2D();
		const auto pPlayer2 = GameSingleton::GetInstance().player2.Get<en::TransformComponent>().transform.GetPosition2D();
		auto deltaPlayerV = pPlayer1 - pPlayer2;
		const en::F32 deltaPlayerD = GameSingleton::GetInstance().playerDelta;
		en::F32 scale = deltaPlayerD / 1024.0f;
		magicBarSprite.setScale(scale, 1.0f);
		magicBarSprite.setPosition(en::toSF(pPlayer2));
		magicBarSprite.setRotation(deltaPlayerV.getPolarAngle());
		target.draw(magicBarSprite);

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

				if (entity.Has<ShipComponent>())
				{
					entity.Get<ShipComponent>().Render(target, states);
				}
				if (entity.Has<en::SpriteComponent>())
				{
					entity.Get<en::SpriteComponent>().sprite.Render(target, states);
				}
				if (entity.Has<en::TextComponent>())
				{
					entity.Get<en::TextComponent>().text.Render(target, states);
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
	auto& gameSing = GameSingleton::GetInstance();
	auto& world = gameSing.world;

	world.GetEntityManager().ClearEntities();
	gameSing.player1 = en::Entity();
	gameSing.player2 = en::Entity();

	en::DataFile file;
	file.LoadFromFile(en::PathManager::GetInstance().GetAssetsPath() + "world.xml");
	file.Deserialize(world, "World");

#ifdef ENLIVE_DEBUG
	world.GetFreeCamView().setCenter(getApplication().GetWindow().getMainView().getSize() * 0.5f);
	world.GetFreeCamView().setSize(getApplication().GetWindow().getMainView().getSize());
	world.GetFreeCamView().setZoom(zoom);
#endif // ENLIVE_DEBUG

	en::Universe::GetInstance().SetCurrentWorld(&world);

	world.GetEntityManager().Each([&world, &gameSing](entt::entity ent)
	{
		en::Entity entity(world, ent);
		if (entity.IsValid() && entity.Has<en::NameComponent>())
		{
			if (!gameSing.player1.IsValid() && entity.Get<en::NameComponent>().name == "Player1")
			{
				gameSing.player1 = entity;
			}
			if (!gameSing.player2.IsValid() && entity.Get<en::NameComponent>().name == "Player2")
			{
				gameSing.player2 = entity;
			}
		}
	});

	if (!gameSing.player1.IsValid())
	{
		auto player1 = world.GetEntityManager().CreateEntity();
		player1.Add<en::NameComponent>().name = "Player1";
		auto& transformPlayer1 = player1.Add<en::TransformComponent>().transform;
		transformPlayer1.SetPosition(gameSing.startPosition + gameSing.startOffsetPlayer1);
		transformPlayer1.SetRotation2D(gameSing.startRotationPlayer1);
		player1.Add<en::RenderableComponent>();
		player1.Add<VelocityComponent>();
		player1.Add<ShipComponent>();
		GameSingleton::GetInstance().player1 = player1;
	}
	else
	{
		auto& transformPlayer1 = gameSing.player1.Get<en::TransformComponent>().transform;
		transformPlayer1.SetPosition(gameSing.startPosition + gameSing.startOffsetPlayer1);
		transformPlayer1.SetRotation2D(gameSing.startRotationPlayer1);
	}
	if (!gameSing.player2.IsValid())
	{
		auto player2 = world.GetEntityManager().CreateEntity();
		player2.Add<en::NameComponent>().name = "Player2";
		auto& transformPlayer2 = player2.Add<en::TransformComponent>().transform;
		transformPlayer2.SetPosition(gameSing.startPosition + gameSing.startOffsetPlayer2);
		transformPlayer2.SetRotation2D(gameSing.startRotationPlayer2);
		player2.Add<en::RenderableComponent>();
		player2.Add<VelocityComponent>();
		player2.Add<ShipComponent>();
		GameSingleton::GetInstance().player2 = player2;
	}
	else
	{
		auto& transformPlayer2 = gameSing.player2.Get<en::TransformComponent>().transform;
		transformPlayer2.SetPosition(gameSing.startPosition + gameSing.startOffsetPlayer2);
		transformPlayer2.SetRotation2D(gameSing.startRotationPlayer2);
	}

	magicBarSprite.setTexture(en::ResourceManager::GetInstance().Get<en::Texture>("spaceMagicBar").Get());
	magicBarSprite.setTextureRect(sf::IntRect(0, 0, 1024, 64));
	magicBarSprite.setOrigin(0.0f, 32.0f);
	magicBarCurrent = 0;
	magicBarAcc = en::Time::Zero();
}

void GameState::Velocity(en::Time dt)
{
	auto& gameSing = GameSingleton::GetInstance();
	auto& world = gameSing.world;

	// Reset Velocity
	auto velocityView = world.GetEntityManager().View<VelocityComponent>();
	for (auto entt : velocityView)
	{
		en::Entity entity(world.GetEntityManager(), entt);
		if (entity.IsValid())
		{
			entity.Get<VelocityComponent>().velocity.set(0.0f, 0.0f);
		}
	}

	// Player mvt
	PlayerMvt(0);
	PlayerMvt(1);


	// SpacePhysic
	/*
	auto spacePhysicView = world.GetEntityManager().View<SpacePhysicComponent, en::TransformComponent>();
	auto velocityView2 = world.GetEntityManager().View<VelocityComponent>();
	for (auto entt : velocityView2)
	{
		en::Entity entity(world.GetEntityManager(), entt);
		if (entity.IsValid())
		{
			auto& vel = entity.Get<VelocityComponent>().velocity;
			auto& transform = entity.Get<en::TransformComponent>().transform;


			
			for (auto enttSpace : spacePhysicView)
			{
				en::Entity spacePhysicEntity(world.GetEntityManager(), enttSpace);
				if (spacePhysicEntity.IsValid())
				{
					auto& spacePhysic = spacePhysicEntity.Get<SpacePhysicComponent>();
					auto& spacePhysicTransform = spacePhysicEntity.Get<en::TransformComponent>().transform;
					const en::Vector2f mvt = spacePhysic.ComputeForce(spacePhysicTransform.GetPosition2D(), transform.GetPosition2D());
					vel += mvt;
				}
			}
		}
	}
	*/

	// Apply
	auto velocityView3 = world.GetEntityManager().View<VelocityComponent, en::TransformComponent>();
	for (auto entt : velocityView3)
	{
		en::Entity entity(world.GetEntityManager(), entt);
		if (entity.IsValid())
		{
			auto& vel = entity.Get<VelocityComponent>();
			auto& transform = entity.Get<en::TransformComponent>();
			if (vel.velocity.getSquaredLength() > 0.5f)
			{
				transform.transform.Move(vel.velocity * 3.0f * dt.AsSeconds());
			}
#ifdef ENLIVE_DEBUG
			if (entity.GetEntity() == GameSingleton::GetInstance().player1.GetEntity())
			{
				GameSingleton::GetInstance().player1Velocity = vel.velocity;
			}
			if (entity.GetEntity() == GameSingleton::GetInstance().player2.GetEntity())
			{
				GameSingleton::GetInstance().player2Velocity = vel.velocity;
			}
#endif // ENLIVE_DEBUG
		}
	}
}

void GameState::PlayerMvt(en::U32 index)
{
	en::Vector2f mvtPlayer;
	if (sf::Joystick::isConnected(index))
	{
		mvtPlayer.x = sf::Joystick::getAxisPosition(index, sf::Joystick::Axis::X);
		mvtPlayer.y = sf::Joystick::getAxisPosition(index, sf::Joystick::Axis::Y);
	}
	else
	{
		if (sf::Keyboard::isKeyPressed((index == 0) ? sf::Keyboard::Z : sf::Keyboard::Up)) mvtPlayer.y -= 100.0f;
		if (sf::Keyboard::isKeyPressed((index == 0) ? sf::Keyboard::S : sf::Keyboard::Down)) mvtPlayer.y += 100.0f;
		if (sf::Keyboard::isKeyPressed((index == 0) ? sf::Keyboard::Q : sf::Keyboard::Left)) mvtPlayer.x -= 100.0f;
		if (sf::Keyboard::isKeyPressed((index == 0) ? sf::Keyboard::D : sf::Keyboard::Right)) mvtPlayer.x += 100.0f;
	}

	const en::F32 angle = (90.0f - mvtPlayer.getPolarAngle());
	auto& player = GameSingleton::GetInstance().GetPlayer(index);
	auto& vel = player.Get<VelocityComponent>().velocity;
	if (player.IsValid())
	{
		// Mvt
		if (mvtPlayer.getSquaredLength() > 1000.0f)
		{
			player.Get<en::TransformComponent>().transform.SetRotation2D(angle);
			vel += mvtPlayer * GameSingleton::GetInstance().energyFactor;
		}

		// PlayerForce
		if (index == 0)
		{
			vel += GameSingleton::GetInstance().playerForceVector;
		}
		else if (index == 1)
		{
			vel -= GameSingleton::GetInstance().playerForceVector;
		}
	}
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

	auto& gameSing = GameSingleton::GetInstance();
	auto& world = gameSing.world;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
	{
		world.GetFreeCamView().setZoom(zoom * 4);
	}
	else
	{
		world.GetFreeCamView().setZoom(zoom);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		getApplication().Stop();
	}

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