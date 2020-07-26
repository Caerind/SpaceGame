#pragma once

#include <Enlivengine/System/Meta.hpp>
#include <Enlivengine/Math/Vector2.hpp>

struct VelocityComponent
{
	VelocityComponent() : velocity() {}
	VelocityComponent(const en::Vector2f& pVelocity) : velocity(pVelocity) {}

	en::Vector2f velocity;
};
ENLIVE_META_CLASS_BEGIN(VelocityComponent)
	ENLIVE_META_CLASS_MEMBER("velocity", &VelocityComponent::velocity)
ENLIVE_META_CLASS_END()

struct PlanetComponent
{
	PlanetComponent() : nothingYet(false) {}

	bool nothingYet;
};
ENLIVE_META_CLASS_BEGIN(PlanetComponent)
ENLIVE_META_CLASS_END()

struct BlackHoleComponent
{
	BlackHoleComponent() : nothingYet(false) {}

	bool nothingYet;
};
ENLIVE_META_CLASS_BEGIN(BlackHoleComponent)
ENLIVE_META_CLASS_END()

struct SpacePhysicComponent
{
	SpacePhysicComponent() : forceFactor(1.0f), dMin(10.0f), dMax(1000.0f) {}

	en::Vector2f ComputeForce(const en::Vector2f& thisPosition, const en::Vector2f& otherPosition)
	{
		en::Vector2f delta = thisPosition - otherPosition;
		const en::F32 dSquared = delta.getSquaredLength();
		if (dSquared > dMin* dMin && dSquared < dMax * dMax && dSquared > 0.01f)
		{
			const en::F32 factor = forceFactor / (dSquared);
			delta.normalize();
			return delta * factor;
		}
		else
		{
			return en::Vector2f();
		}
	}

	en::F32 forceFactor;
	en::F32 dMin;
	en::F32 dMax;
};
ENLIVE_META_CLASS_BEGIN(SpacePhysicComponent)
	ENLIVE_META_CLASS_MEMBER("forceFactor", &SpacePhysicComponent::forceFactor),
	ENLIVE_META_CLASS_MEMBER("dMin", &SpacePhysicComponent::dMin),
	ENLIVE_META_CLASS_MEMBER("dMax", &SpacePhysicComponent::dMax)
ENLIVE_META_CLASS_END()

struct ShipComponent
{
	ShipComponent() 
		: body()
		, bodyFrame()
		, wings()
		, wingsFrame()
		, weapons()
		, engine()
		, shield()
	{}

	void Render(sf::RenderTarget& target, sf::RenderStates states) const
	{
		wings.Render(target, states);
		wingsFrame.Render(target, states);
		engine.Render(target, states);
		weapons.Render(target, states);
		body.Render(target, states);
		bodyFrame.Render(target, states);
		shield.Render(target, states);
	}

	en::Sprite body;
	en::Sprite bodyFrame;
	en::Sprite wings;
	en::Sprite wingsFrame;
	en::Sprite weapons;
	en::Sprite engine;
	en::Sprite shield;
};
ENLIVE_META_CLASS_BEGIN(ShipComponent)
	ENLIVE_META_CLASS_MEMBER("body", &ShipComponent::body),
	ENLIVE_META_CLASS_MEMBER("bodyFrame", &ShipComponent::bodyFrame),
	ENLIVE_META_CLASS_MEMBER("wings", &ShipComponent::wings),
	ENLIVE_META_CLASS_MEMBER("wingsFrame", &ShipComponent::wingsFrame),
	ENLIVE_META_CLASS_MEMBER("weapons", &ShipComponent::weapons),
	ENLIVE_META_CLASS_MEMBER("engine", &ShipComponent::engine),
	ENLIVE_META_CLASS_MEMBER("shield", &ShipComponent::shield)
ENLIVE_META_CLASS_END()