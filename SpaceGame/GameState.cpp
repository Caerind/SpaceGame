#include "GameState.hpp"

#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Core/Universe.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <Enlivengine/Tools/ImGuiEntityBrowser.hpp>
#include <Enlivengine/Graphics/DebugDraw.hpp>
#include <Enlivengine/Math/Random.hpp>

#include <Enlivengine/Core/Components.hpp>
#include "Components.hpp"

#include "GameSingleton.hpp"
#include "AudioTrackMixer3000.hpp"

GameState::GameState(en::StateManager& manager)
	: en::State(manager)
{
	Init();
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

	AudioTrackMixer3000::GetInstance().UpdateVolumes();

	if (!GameSingleton::GetInstance().world.IsPlaying())
	{
		return false;
	}

	GameSingleton::GetInstance().UpdateStats(dt);

	Velocity(dt);

	// Shoot player
	PlayerShoot(dt, 0);
	PlayerShoot(dt, 1);

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
	static bool transformDebugInit = false;
	static sf::RectangleShape tX;
	static sf::RectangleShape tY;
	if (!transformDebugInit)
	{
		static constexpr en::F32 scale = 10.0f;
		tX.setSize({ 10.0f, 1.0f });
		tX.setFillColor(sf::Color::Green);
		tX.setOrigin({ 0.0f, 0.5f });
		tX.setScale({ 10.0f, 10.0f });
		tY.setSize({ 1.0f, 10.0f });
		tY.setFillColor(sf::Color::Red);
		tY.setOrigin({ 0.5f, 0.0f });
		tY.setScale({ 10.0f, 10.0f });
		transformDebugInit = true;
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

				if (entity.Has<PlanetComponent>())
				{
					auto& p = entity.Get<PlanetComponent>();
					p.atm.Render(target, states);
					p.core.Render(target, states);
					p.tex.Render(target, states);
					p.shad.Render(target, states);
				}
				if (entity.Has<ShipComponent>())
				{
					entity.Get<ShipComponent>().Render(target, states);

#ifdef ENLIVE_DEBUG
					// Debug Collision
					sf::CircleShape coll;
					coll.setOutlineThickness(1.5f);
					coll.setOutlineColor(en::toSF(en::Color::Red));
					coll.setFillColor(en::toSF(en::Color::Transparent));
					coll.setRadius(128.0f * 0.5f);
					coll.setPosition(en::toSF(entity.GetPosition2D()));
					coll.setOrigin(128.0f * 0.5f, 128.0f * 0.5f);
					target.draw(coll);
#endif
				}
				if (entity.Has<en::SpriteComponent>())
				{
					entity.Get<en::SpriteComponent>().sprite.Render(target, states);
				}
				if (entity.Has<en::TextComponent>())
				{
					entity.Get<en::TextComponent>().text.Render(target, states);
				}
				if (entity.Has<BarkComponent>())
				{
					sf::RenderStates statesBark;
					statesBark.transform.translate(en::toSF(entity.GetPosition2D()));
					entity.Get<BarkComponent>().text.Render(target, statesBark);
				}

#ifdef ENLIVE_DEBUG
				if (en::ImGuiEntityBrowser::GetInstance().IsSelected(entity))
				{
					target.draw(tX, states);
					target.draw(tY, states);
				}
#endif // ENLIVE_DEBUG
			}
		}
	}

#ifdef ENLIVE_DEBUG
	static sf::CircleShape shape;
	const en::F32 radius = GameSingleton::GetInstance().distanceMax;
	shape.setFillColor(sf::Color::Transparent);
	shape.setOutlineColor(sf::Color::Red);
	shape.setOutlineThickness(10.0f);
	shape.setRadius(radius);
	shape.setOrigin(radius, radius);
	target.draw(shape);
#endif // ENLIVE_DEBUG

	// UI
	{
		static sf::Vector2f idealSize = { 1920.0f, 1080.f };
		static sf::View uiViewRect(idealSize * 0.5f, idealSize);
		target.setView(uiViewRect);
		ENLIVE_PROFILE_SCOPE(UI);
		auto uiView = world.GetEntityManager().View<UIComponent>();
		for (auto entt : uiView)
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

	gameSing.distanceMax = gameSing.distanceMaxInitial;

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
		player1.Add<BarkComponent>();
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
		player2.Add<BarkComponent>();
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

	AudioTrackMixer3000::GetInstance().Play();

	GameSingleton::GetInstance().UpdateStats(en::Time::Zero());
	AudioTrackMixer3000::GetInstance().UpdateVolumes();
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
	PlayerMvt(dt, 0);
	PlayerMvt(dt, 1);
	AIMvt(dt);
	ShootUpdate(dt);


	// SpacePhysic
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

void GameState::PlayerMvt(en::Time dt, en::U32 index)
{
	auto& player = GameSingleton::GetInstance().GetPlayer(index);
	if (player.IsValid())
	{
		auto& pla = player.Get<ShipComponent>();
		auto& vel = player.Get<VelocityComponent>().velocity;
		auto& tra = player.Get<en::TransformComponent>().transform;

		if (sf::Joystick::isConnected(index))
		{
#ifdef ENLIVE_DEBUG
			auto& player2 = GameSingleton::GetInstance().GetPlayer(1);
			auto& tra2 = player2.Get<en::TransformComponent>().transform;
			enLogInfo(0, "p1:{}   -   p2:{}", tra.GetRotation2D(), tra2.GetRotation2D());
#endif // ENLIVE_DEBUG

			en::Vector2f polarAngle;
			en::Vector2f dirPlayer;
			dirPlayer.x = sf::Joystick::getAxisPosition(index, sf::Joystick::Axis::X);
			dirPlayer.y = sf::Joystick::getAxisPosition(index, sf::Joystick::Axis::Y);
			if (dirPlayer.getSquaredLength() > 1000.0f)
			{
				const en::F32 dirAngle = 90 - dirPlayer.getPolarAngle();
				polarAngle = en::Vector2f::polar(dirAngle, 1.0f);

				const en::F32 angleBegin = -(270 + tra.GetRotation2D()) - 90;
				const en::Vector2f polarAngleBegin = en::Vector2f::polar(angleBegin, 1.0f);

				const en::F32 lerpPercent = GameSingleton::GetInstance().degPerSecond * dt.AsSeconds(); // TODO : pla.GetAgility();
				polarAngle.x = en::Math::Lerp(polarAngleBegin.x, polarAngle.x, lerpPercent);
				polarAngle.y = en::Math::Lerp(polarAngleBegin.y, polarAngle.y, lerpPercent);

				// Angle
				const en::F32 finalAngle = en::Math::Equals(polarAngle.y, 0.0f) ? 0.0f : polarAngle.getPolarAngle();

				enLogInfo(0, "{} {} {}", dirAngle, angleBegin, finalAngle);

				tra.SetRotation2D(180.0f - finalAngle);
			}
			else
			{
				polarAngle = en::Vector2f::polar(tra.GetRotation2D(), 1.0f);
				enLogInfo(0, "{}", tra.GetRotation2D());
			}

			// Mvt
			const bool moveForward = sf::Joystick::isButtonPressed(index, 0);
			if (moveForward)
			{
				vel += polarAngle * 100.0f * GameSingleton::GetInstance().energyFactor;
			}
		}
		else
		{
			en::Vector2f mvtPlayer;

			if (index == 0)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
					mvtPlayer.y -= 100.0f;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
					mvtPlayer.y += 100.0f;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
					mvtPlayer.x -= 100.0f;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
					mvtPlayer.x += 100.0f;
			}
			else
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
					mvtPlayer.y -= 100.0f;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
					mvtPlayer.y += 100.0f;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
					mvtPlayer.x -= 100.0f;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
					mvtPlayer.x += 100.0f;
			}

			if (mvtPlayer.getSquaredLength() > 1000.0f)
			{
				const en::F32 angle = (90.0f - mvtPlayer.getPolarAngle());
				enLogInfo(0, "p2Angle{} - p2MvtAngle{}", angle, mvtPlayer.getPolarAngle());
				// Mvt
				vel += mvtPlayer * GameSingleton::GetInstance().energyFactor;
				// Angle
				player.Get<en::TransformComponent>().transform.SetRotation2D(angle);
			}
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

void GameState::AIMvt(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

	const auto& aiParams = GameSingleton::GetInstance().aiParams;
	auto& entityManager = GameSingleton::GetInstance().world.GetEntityManager();

	auto aiView = entityManager.View<VelocityComponent, en::TransformComponent, AIComponent, ShipComponent>();
	for (auto ent : aiView)
	{
		en::Entity entity(entityManager, ent);
		if (entity.IsValid())
		{
			auto& ai = entity.Get<AIComponent>();
			auto& ship = entity.Get<ShipComponent>();

			auto p1 = entity.GetPosition2D();

			bool killed = false;
			auto projView = entityManager.View<ShootComponent>();
			for (auto pEnt : projView)
			{
				if (!killed)
				{
					en::Entity pEntity = en::Entity(entityManager, pEnt);
					if (pEntity.IsValid())
					{
						auto d = pEntity.GetPosition2D() - p1;
						if (d.getSquaredLength() < 128.0f * 128.0f)
						{
							ship.damage -= 0.25f;
							entityManager.DestroyEntity(pEntity);

							if (ship.damage <= 0.0f)
							{
								entityManager.DestroyEntity(entity);
								killed = true;
							}
						}
					}
				}
			}

			if (!killed)
			{
				auto dp = (GameSingleton::GetInstance().posCenter - p1);

				ai.cooldown -= dt;
				if (ai.cooldown <= en::Time::Zero() && dp.getSquaredLength() < aiParams.plD * aiParams.plD)
				{
					ai.cooldown = aiParams.cooldown + en::Time::Seconds(en::Random::getDev(0.0f, aiParams.cooldownDev.AsSeconds()));
					Shoot(entity);
				}

				ai.recompute -= dt;
				if (ai.recompute <= en::Time::Zero())
				{
					ai.recompute = en::Time::Seconds(en::Random::get(aiParams.timeRecompMin.AsSeconds(), aiParams.timeRecompMax.AsSeconds()));


					en::Vector2f ev; // Evitement
					en::Vector2f al; // Alignment

					auto aiView2 = entityManager.View<AIComponent>();
					for (auto ent2 : aiView2)
					{
						en::Entity entity2(entityManager, ent2);
						if (entity2.IsValid() && ent != ent2)
						{
							auto& ai2 = entity2.Get<AIComponent>();
							if (ai2.swarmIndex == ai.swarmIndex)
							{
								auto p2 = entity2.GetPosition2D();
								auto delta = p2 - p1;

								if (delta.getSquaredLength() < aiParams.evD * aiParams.evD)
								{
									ev += delta;
								}

								al += en::Vector2f::polar(ai2.angle, 1.0f);
							}
						}
					}
					if (ev.getSquaredLength() > 0.01f)
					{
						ev.normalize();
					}
					if (al.getSquaredLength() > 0.01f)
					{
						al.normalize();
					}

					// Player pos
					en::Vector2f pl = dp.normalized();
					auto p = aiParams.plF;
					if (dp.getSquaredLength() > aiParams.plD* aiParams.plD)
					{
						p = 0.0f;
					}

					// Initial pos
					auto di = ai.initPos - p1;
					auto pi = di.normalized();
					auto fi = aiParams.plI;
					if (di.getSquaredLength() > aiParams.plD* aiParams.plD)
					{
						fi = 0.0f;
					}

					// Random
					en::Vector2f rn = en::Vector2f::polar(ai.angle + en::Random::getDev(0.0f, aiParams.randDev), 1.0f);

					en::Vector2f final = -aiParams.evF * ev + aiParams.alF * al + p * pl + aiParams.rnF * rn + fi * pi;
					final.normalize();
					ai.angle = final.getPolarAngle();
				}

				entity.Get<en::TransformComponent>().transform.SetRotation2D(90 - ai.angle);
				entity.Get<VelocityComponent>().velocity += en::Vector2f::polar(ai.angle, 90.0f);
			}
		}
	}
}

void GameState::ShootUpdate(en::Time dt)
{
	auto& entityManager = GameSingleton::GetInstance().world.GetEntityManager();
	auto velocityView = entityManager.View<VelocityComponent, ShootComponent>();
	for (auto entt : velocityView)
	{
		en::Entity entity(entityManager, entt);
		if (entity.IsValid())
		{
			auto& shoot = entity.Get<ShootComponent>();
			entity.Get<VelocityComponent>().velocity += shoot.direction * shoot.speed;
			shoot.duration -= dt;

			if (shoot.duration <= en::Time::Zero())
			{
				entityManager.DestroyEntity(entity);
			}
		}
	}
}

void GameState::PlayerShoot(en::Time dt, en::U32 index)
{
	bool fire = false;
	static en::Time cooldown = en::Time::Zero();
	cooldown += dt;
	if (index == 0)
	{
		fire = getApplication().GetActionSystem().IsInputActive("player1Fire");
	}
	else
	{
		fire = getApplication().GetActionSystem().IsInputActive("player2Fire");
	}
	if (fire && cooldown > en::Time::Seconds(0.1f))
	{
		cooldown = en::Time::Zero();
		en::Entity entity = GameSingleton::GetInstance().GetPlayer(index);
		if (entity.IsValid())
		{
			Shoot(entity);
		}
	}
}

void GameState::Shoot(const en::Entity& entity)
{
	if (entity.IsValid() && entity.Has<en::TransformComponent>())
	{
		auto& t = entity.Get<en::TransformComponent>().transform;
		auto p = t.GetPosition2D();
		auto angle = -(270.0f + t.GetRotation2D());
		auto r = en::Vector2f::polar(angle, 1.0f);
		auto f = p + r * 200.0f;

		en::Entity proj = GameSingleton::GetInstance().world.GetEntityManager().CreateEntity();
		if (proj.IsValid())
		{
#ifdef ENLIVE_DEBUG
			proj.Add<en::NameComponent>().name = "Projectile";
#endif
			proj.Add<en::RenderableComponent>();
			proj.Add<VelocityComponent>();
			auto& tProj = proj.Add<en::TransformComponent>().transform;
			tProj.SetRotation2D(90.0f - angle);
			tProj.SetPosition(f);
			auto& sProj = proj.Add<en::SpriteComponent>().sprite;
			sProj.SetTexture(en::ResourceManager::GetInstance().Get<en::Texture>("sheet").Get());
			sProj.SetTextureRect(en::Recti(856, 869, 9, 57));
			sProj.SetOrigin(0.5f * sProj.GetTextureRect().getSize().x, 0.5f * sProj.GetTextureRect().getSize().y);
			auto& pProj = proj.Add<ShootComponent>();
			pProj.direction = r;
			pProj.speed = 300.0f;
			pProj.duration = en::Time::Seconds(3.0f);
			pProj.shooter = static_cast<en::U32>(entity.GetEntity());
		}
	}
}

void GameState::SpawnSwarm(en::U32 swarmIndex, const en::Vector2f& pos)
{
	en::F32 rot = en::Random::get(0.0f, 360.0f);

	SpawnEnemy(pos + en::Vector2f(-200.0f, 100.f), rot, swarmIndex, en::Color::Red);
	SpawnEnemy(pos + en::Vector2f(200.0f, 100.f), rot + 10, swarmIndex, en::Color::Red);
	SpawnEnemy(pos + en::Vector2f(0.0f, -200.f), rot - 10, swarmIndex, en::Color::Red);
}

void GameState::SpawnEnemy(const en::Vector2f& pos, en::F32 rotation, en::U32 swarmIndex, const en::Color& color)
{
	static en::U32 id;
	id++;
	auto enemy = GameSingleton::GetInstance().world.GetEntityManager().CreateEntity();
	enemy.Add<en::NameComponent>().name = "Enemy" + en::ToString(id);
	auto& transform = enemy.Add<en::TransformComponent>().transform;
	transform.SetPosition(pos);
	transform.SetRotation2D(90 + rotation);
	transform.SetScale(0.5f);
	enemy.Add<en::RenderableComponent>();
	enemy.Add<VelocityComponent>();
	auto& ship = enemy.Add<ShipComponent>();
	ship.body.SetTexture(en::ResourceManager::GetInstance().Get<en::Texture>("shipDefault").Get());
	ship.body.SetOrigin(128, 128);
	enemy.Add<BarkComponent>();
	auto& ai = enemy.Add<AIComponent>();
	ai.initPos = pos;
	ai.swarmIndex = swarmIndex;
}

void GameState::SpawnPlanet(const en::Vector2f& pos)
{
	static en::U32 id;
	id++;
	auto planet = GameSingleton::GetInstance().world.GetEntityManager().CreateEntity();
	planet.Add<en::NameComponent>().name = "Planet" + en::ToString(id);
	auto& transform = planet.Add<en::TransformComponent>().transform;
	transform.SetPosition(en::Vector3f(pos, -1000.0f));
	transform.SetScale(2.0f);
	planet.Add<en::RenderableComponent>();
	auto& planetC = planet.Add<PlanetComponent>();
	en::Texture& texture = en::ResourceManager::GetInstance().Get<en::Texture>("planets").Get();
	if (!texture.isSmooth())
	{
		texture.setSmooth(true);
	}
	planetC.atm.SetTexture(texture);
	planetC.atm.SetTextureRect(en::Recti(en::Random::get(0, 3) * 320, 0, 320, 320));
	planetC.atm.SetOrigin(160.0f, 160.0f);
	planetC.core.SetTexture(texture);
	planetC.core.SetTextureRect(en::Recti(en::Random::get(0, 3) * 320, 320, 320, 320));
	planetC.core.SetOrigin(160.0f, 160.0f);
	planetC.tex.SetTexture(texture);
	planetC.tex.SetTextureRect(en::Recti(en::Random::get(0, 3) * 320, 640, 320, 320));
	planetC.tex.SetOrigin(160.0f, 160.0f);
	planetC.shad.SetTexture(texture);
	planetC.shad.SetTextureRect(en::Recti(en::Random::get(0, 3) * 320, 960, 320, 320));
	planetC.shad.SetOrigin(160.0f, 160.0f);
	auto& phys = planet.Add<SpacePhysicComponent>();
	phys.forceFactor = 20000000.0f;
	phys.dMin = 400.0f;
	phys.dMax = 3000.0f;
}

#ifdef ENLIVE_DEBUG
void GameState::DebugUpdate(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

	auto& gameSing = GameSingleton::GetInstance();
	auto& world = gameSing.world;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
	{
		world.GetFreeCamView().setZoom(zoom * 4);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
	{
		world.GetFreeCamView().setZoom(zoom * 0.3f);
	}
	else
	{
		world.GetFreeCamView().setZoom(zoom);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		getApplication().Stop();
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		if (GameSingleton::GetInstance().world.IsPlaying())
		{
			GameSingleton::GetInstance().world.Pause();
		}
		else
		{
			GameSingleton::GetInstance().world.Play();
		}
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

#ifdef ENLIVE_DEBUG
	if (getApplication().GetActionSystem().IsInputActive("save"))
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
	if (getApplication().GetActionSystem().IsInputActive("rightMouseButton"))
	{
		auto mp = getApplication().GetWindow().getCursorPositionView(GameSingleton::GetInstance().world.GetFreeCamView().getHandle());

		static en::U32 index = 0;
		index++;
		SpawnSwarm(index, mp);
	}
	if (getApplication().GetActionSystem().IsInputActive("middleMouseButton"))
	{
		auto mp = getApplication().GetWindow().getCursorPositionView(GameSingleton::GetInstance().world.GetFreeCamView().getHandle());
		SpawnPlanet(mp);
	}
#endif // ENLIVE_DEBUG
}
#endif // ENLIVE_DEBUG