#pragma once

#include <Enlivengine/Graphics/View.hpp>
#include <Enlivengine/Core/EntityManager.hpp>
#include <Enlivengine/Core/PhysicSystem.hpp>

namespace en
{

class World
{
public:
	World();
	~World();

	EntityManager& GetEntityManager();
	const EntityManager& GetEntityManager() const;

	template <typename T = PhysicSystem>
	bool CreatePhysicSystem();
	bool HasPhysicSystem() const;
	PhysicSystem* GetPhysicSystem();
	const PhysicSystem* GetPhysicSystem() const;

	View& GetGameView();
	const View& GetGameView() const;

#ifdef ENLIVE_DEBUG
	View& GetFreeCamView();
	const View& GetFreeCamView() const;
#endif // ENLIVE_DEBUG

	void Play();
	void Pause();
	bool IsPlaying() const;
	
	void Update(Time dt);
	void Render(sf::RenderTarget& target);

private:
	EntityManager mEntityManager;
	PhysicSystem* mPhysicSystem;

	View mGameView;

#ifdef ENLIVE_DEBUG
	View mFreeCamView;
#endif // ENLIVE_DEBUG

	bool mPlaying;
};

template <typename T>
bool World::CreatePhysicSystem()
{
	mPhysicSystem = new T(*this);
	return mPhysicSystem != nullptr;
}

} // namespace en

ENLIVE_META_CLASS_BEGIN(en::World)
	ENLIVE_META_CLASS_MEMBER("EntityManager", &en::World::GetEntityManager)
ENLIVE_META_CLASS_END()
