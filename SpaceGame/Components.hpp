#pragma once

#include <Enlivengine/System/Meta.hpp>
#include <Enlivengine/Math/Vector2.hpp>

struct BodyPart
{
	en::Recti body;
	en::Recti bodyColor;
	std::array<en::Recti,4> damages;
	en::F32 agility;
	en::F32 damage;
	en::F32 resistance;
	en::F32 speed;
};
ENLIVE_META_CLASS_BEGIN(BodyPart)
ENLIVE_META_CLASS_MEMBER("body", &BodyPart::body),
ENLIVE_META_CLASS_MEMBER("bodyColor", &BodyPart::bodyColor),
ENLIVE_META_CLASS_MEMBER("damages", &BodyPart::damages),
ENLIVE_META_CLASS_MEMBER("agility", &BodyPart::agility),
ENLIVE_META_CLASS_MEMBER("damage", &BodyPart::damage),
ENLIVE_META_CLASS_MEMBER("resistance", &BodyPart::resistance),
ENLIVE_META_CLASS_MEMBER("speed", &BodyPart::speed)
ENLIVE_META_CLASS_END()

struct WingsPart
{
	en::Recti wings;
	en::Recti wingsColor;
	std::array<en::Recti, 4> damages;
	en::F32 agility;
	en::F32 damage;
	en::F32 resistance;
	en::F32 speed;
};
ENLIVE_META_CLASS_BEGIN(WingsPart)
ENLIVE_META_CLASS_MEMBER("wings", &WingsPart::wings),
ENLIVE_META_CLASS_MEMBER("wingsColor", &WingsPart::wingsColor),
ENLIVE_META_CLASS_MEMBER("damages", &WingsPart::damages),
ENLIVE_META_CLASS_MEMBER("agility", &WingsPart::agility),
ENLIVE_META_CLASS_MEMBER("damage", &WingsPart::damage),
ENLIVE_META_CLASS_MEMBER("resistance", &WingsPart::resistance),
ENLIVE_META_CLASS_MEMBER("speed", &WingsPart::speed)
ENLIVE_META_CLASS_END()

struct CanonPart
{
	en::Recti texture;
	en::F32 agility;
	en::F32 damage;
	en::F32 resistance;
	en::F32 speed;
};
ENLIVE_META_CLASS_BEGIN(CanonPart)
ENLIVE_META_CLASS_MEMBER("texture", &CanonPart::texture),
ENLIVE_META_CLASS_MEMBER("agility", &CanonPart::agility),
ENLIVE_META_CLASS_MEMBER("damage", &CanonPart::damage),
ENLIVE_META_CLASS_MEMBER("resistance", &CanonPart::resistance),
ENLIVE_META_CLASS_MEMBER("speed", &CanonPart::speed)
ENLIVE_META_CLASS_END()

struct EnginePart
{
	en::Recti texture;
	en::F32 agility;
	en::F32 damage;
	en::F32 resistance;
	en::F32 speed;
};
ENLIVE_META_CLASS_BEGIN(EnginePart)
ENLIVE_META_CLASS_MEMBER("texture", &EnginePart::texture),
ENLIVE_META_CLASS_MEMBER("agility", &EnginePart::agility),
ENLIVE_META_CLASS_MEMBER("damage", &EnginePart::damage),
ENLIVE_META_CLASS_MEMBER("resistance", &EnginePart::resistance),
ENLIVE_META_CLASS_MEMBER("speed", &EnginePart::speed)
ENLIVE_META_CLASS_END()

struct ArmPart
{
	en::Recti texture;
	en::F32 agility;
	en::F32 damage;
	en::F32 resistance;
	en::F32 speed;
};
ENLIVE_META_CLASS_BEGIN(ArmPart)
ENLIVE_META_CLASS_MEMBER("texture", &ArmPart::texture),
ENLIVE_META_CLASS_MEMBER("agility", &ArmPart::agility),
ENLIVE_META_CLASS_MEMBER("damage", &ArmPart::damage),
ENLIVE_META_CLASS_MEMBER("resistance", &ArmPart::resistance),
ENLIVE_META_CLASS_MEMBER("speed", &ArmPart::speed)
ENLIVE_META_CLASS_END()

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
	PlanetComponent() {}

	en::Sprite atm;
	en::Sprite core;
	en::Sprite tex;
	en::Sprite shad;
};
ENLIVE_META_CLASS_BEGIN(PlanetComponent)
ENLIVE_META_CLASS_MEMBER("atm", &PlanetComponent::atm),
ENLIVE_META_CLASS_MEMBER("core", &PlanetComponent::core),
ENLIVE_META_CLASS_MEMBER("tex", &PlanetComponent::tex),
ENLIVE_META_CLASS_MEMBER("shad", &PlanetComponent::shad)
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
		const en::F32 dMinSquared = dMin * dMin;
		const en::F32 dMaxSquared = dMax * dMax;
		if (dSquared > dMinSquared && dSquared < dMaxSquared && dSquared > 0.01f)
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

struct PlayerComponent
{
	PlayerComponent() : nothingYet(false) {}

	bool nothingYet;
};
ENLIVE_META_CLASS_BEGIN(PlayerComponent)
ENLIVE_META_CLASS_END()

struct AIComponent
{
	AIComponent() : recompute(en::Time::Seconds(0.1f)), cooldown(en::Time::Seconds(0.2f)), initPos(), swarmIndex(0), angle(0.0f) {}

	en::Time recompute;
	en::Time cooldown;
	en::Vector2f initPos;
	en::U32 swarmIndex;
	en::F32 angle;
};
ENLIVE_META_CLASS_BEGIN(AIComponent)
	ENLIVE_META_CLASS_MEMBER("recompute", &AIComponent::recompute),
	ENLIVE_META_CLASS_MEMBER("cooldown", &AIComponent::cooldown),
	ENLIVE_META_CLASS_MEMBER("initPos", &AIComponent::initPos),
	ENLIVE_META_CLASS_MEMBER("swarmIndex", &AIComponent::swarmIndex),
	ENLIVE_META_CLASS_MEMBER("angle", &AIComponent::angle)
ENLIVE_META_CLASS_END()

struct ShootComponent
{
	ShootComponent() : direction(), speed(1.0f), duration(en::Time::Zero()), shooter(static_cast<en::U32>(en::U32_Max)) {}

	en::Vector2f direction;
	en::F32 speed;
	en::Time duration;
	en::U32 shooter; // entt::entity
};
ENLIVE_META_CLASS_BEGIN(ShootComponent)
	ENLIVE_META_CLASS_MEMBER("direction", &ShootComponent::direction),
	ENLIVE_META_CLASS_MEMBER("speed", &ShootComponent::speed),
	ENLIVE_META_CLASS_MEMBER("duration", &ShootComponent::duration),
	ENLIVE_META_CLASS_MEMBER("shooter", &ShootComponent::shooter)
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
		, damage(1.0f)
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

	en::F32 damage;
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

struct UIComponent
{
	UIComponent() : nothingYet(false) {}

	bool nothingYet;
};
ENLIVE_META_CLASS_BEGIN(UIComponent)
ENLIVE_META_CLASS_END()

struct BarkComponent
{
	BarkComponent() : text() {}

	en::Text text;
};
ENLIVE_META_CLASS_BEGIN(BarkComponent)
	ENLIVE_META_CLASS_MEMBER("text", &BarkComponent::text)
ENLIVE_META_CLASS_END()