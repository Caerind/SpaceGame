#pragma once

#include <Box2D/Box2D.h>

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/Math/Vector2.hpp>

#include <Enlivengine/Core/CustomTraits.hpp>
#include <Enlivengine/Core/World.hpp>

namespace en
{

class Entity;

enum class BodyType
{
	Dynamic,
	Static,
	Kinematic
};

class PhysicComponent
{
    public:
        PhysicComponent();
        ~PhysicComponent();
        
        bool IsValid() const;
        
        void SetBodyType(BodyType type);
        BodyType GetBodyType() const;
        
        void SetLinearVelocity(const Vector2f& velocity);
        Vector2f GetLinearVelocity() const;
        
        void SetAngularVelocity(F32 velocity);
        F32 GetAngularVelocity() const;
        
        void SetGravityScale(F32 scale);
        F32 GetGravityScale() const;
        
        void SetLinearDamping(F32 value);
        F32 GetLinearDamping() const;
        
        void SetAngularDamping(F32 value);
        F32 GetAngularDamping() const;
        
        void SetFixedRotation(bool value);
        bool IsFixedRotation() const;
        
        void SetBullet(bool value);
        bool IsBullet() const;
        
        // TODO : AllowSleeping/Awake/Enable
        
        // TODO : Fixtures
        
    private:
		friend class PhysicSystem;

        b2Body* mBody;
};

} // namespace en

ENLIVE_DEFINE_TYPE_INFO(BodyType)

ENLIVE_META_CLASS_BEGIN(en::PhysicComponent)
	ENLIVE_META_CLASS_MEMBER("type", &en::PhysicComponent::GetBodyType, &en::PhysicComponent::SetBodyType),
	ENLIVE_META_CLASS_MEMBER("linearVelocity", &en::PhysicComponent::GetLinearVelocity, &en::PhysicComponent::SetLinearVelocity),
	ENLIVE_META_CLASS_MEMBER("angularVelocity", &en::PhysicComponent::GetAngularVelocity, &en::PhysicComponent::SetAngularVelocity),
	ENLIVE_META_CLASS_MEMBER("gravityScale", &en::PhysicComponent::GetGravityScale, &en::PhysicComponent::SetGravityScale),
	ENLIVE_META_CLASS_MEMBER("linearDamping", &en::PhysicComponent::GetLinearDamping, &en::PhysicComponent::SetLinearDamping),
	ENLIVE_META_CLASS_MEMBER("angularDamping", &en::PhysicComponent::GetAngularDamping, &en::PhysicComponent::SetAngularDamping),
	ENLIVE_META_CLASS_MEMBER("fixedRotation", &en::PhysicComponent::IsFixedRotation, &en::PhysicComponent::SetFixedRotation),
	ENLIVE_META_CLASS_MEMBER("bullet", &en::PhysicComponent::IsBullet, &en::PhysicComponent::SetBullet)
ENLIVE_META_CLASS_END()

template <>
struct CustomComponentInitialization<en::PhysicComponent>
{
	static constexpr bool value = true;
	static bool Initialize(const en::Entity& entity, en::PhysicComponent& component)
	{
		en::World& world = const_cast<en::World&>(entity.GetWorld());
		if (world.HasPhysicSystem())
		{
			return world.GetPhysicSystem()->Initialize(entity, component);
		}
		else
		{
			return false;
		}
	}
};