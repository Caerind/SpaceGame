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