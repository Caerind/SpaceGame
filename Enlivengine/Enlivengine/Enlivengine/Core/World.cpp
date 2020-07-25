#include <Enlivengine/Core/World.hpp>

#include <Enlivengine/Tools/ImGuiEntityBrowser.hpp> // EntityBrowser selection
#include <Enlivengine/System/Profiler.hpp> // Profiling : Render(), Update()
#include <SFML/Graphics/CircleShape.hpp> // RenderSystem
#include <Enlivengine/Core/Components.hpp> // Components

namespace en
{

World::World()
	: mEntityManager(*this)
	, mGameView()
#ifdef ENLIVE_DEBUG
	, mFreeCamView()
#endif // ENLIVE_DEBUG
	, mPlaying(false)
{
}

EntityManager& World::GetEntityManager()
{
	return mEntityManager;
}

const EntityManager& World::GetEntityManager() const
{
	return mEntityManager;
}

View& World::GetGameView()
{
	return mGameView;
}

const View& World::GetGameView() const
{
	return mGameView;
}

#ifdef ENLIVE_DEBUG
View& World::GetFreeCamView()
{
	return mFreeCamView;
}

const View& World::GetFreeCamView() const
{
	return mFreeCamView;
}
#endif // ENLIVE_DEBUG

void World::Play()
{
	mPlaying = true;
}

void World::Pause()
{
	mPlaying = false;
}

bool World::IsPlaying() const
{
	return mPlaying;
}

void World::SetIDGenerator(U32 idGenerator)
{
	mIDGenerator = idGenerator;
}

U32 World::GetIDGenerator() const
{
	return mIDGenerator;
}

U32 World::CreateID()
{
	return mIDGenerator++;
}

void World::Render(sf::RenderTarget& target)
{
	ENLIVE_PROFILE_FUNCTION();

	const sf::View previousView = target.getView();

#ifdef ENLIVE_DEBUG
	bool editor = true;
	if (editor)
	{
		target.setView(GetFreeCamView().getHandle());
	}
	else
	{
		target.setView(GetGameView().getHandle());
	}
#else
	target.setView(GetGameView().getHandle());
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
		GetEntityManager().GetRegistry().sort<en::TransformComponent>([](const auto& lhs, const auto& rhs)
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
		auto transformView = GetEntityManager().View<en::RenderableComponent>();
		for (auto entt : transformView)
		{
			en::Entity entity(GetEntityManager(), entt);
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

} // namespace en