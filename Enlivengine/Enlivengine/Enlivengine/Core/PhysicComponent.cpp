#include "PhysicComponent.hpp"

#include "World.hpp"
#include "Entity.hpp"

namespace en
{
	
PhysicComponent::PhysicComponent()
	: mBody(nullptr)
{
}

PhysicComponent::~PhysicComponent()
{
}

bool PhysicComponent::IsValid() const
{
	return mBody != nullptr;
}

void PhysicComponent::SetBodyType(BodyType type)
{
	if (IsValid())
	{
		switch (type)
		{
		case BodyType::Dynamic: mBody->SetType(b2_dynamicBody); break;
		case BodyType::Static: mBody->SetType(b2_staticBody); break;
		case BodyType::Kinematic: mBody->SetType(b2_kinematicBody); break;
		default: enAssert(false);
		}
	}
}

BodyType PhysicComponent::GetBodyType() const
{
	if (IsValid())
	{
		const b2BodyType type = mBody->GetType();
		switch (type)
		{
		case b2_dynamicBody: return BodyType::Dynamic; break;
		case b2_staticBody: return BodyType::Static; break;
		case b2_kinematicBody: return BodyType::Kinematic; break;
		default: enAssert(false);
		}
	}
	return BodyType::Static;
}

void PhysicComponent::SetLinearVelocity(const Vector2f& velocity)
{
	if (IsValid())
	{
		mBody->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
	}
}

Vector2f PhysicComponent::GetLinearVelocity() const
{
	if (IsValid())
	{
		const b2Vec2 velocity = mBody->GetLinearVelocity();
		return Vector2f(velocity.x, velocity.y);
	}
	else
	{
		return Vector2f(0.0f, 0.0f);
	}
}

void PhysicComponent::SetAngularVelocity(F32 velocity)
{
	if (IsValid())
	{
		mBody->SetAngularVelocity(velocity);
	}
}

F32 PhysicComponent::GetAngularVelocity() const
{
	if (IsValid())
	{
		return mBody->GetAngularVelocity();
	}
	else
	{
		return 0.0f;
	}
}

void PhysicComponent::SetGravityScale(F32 scale)
{
	if (IsValid())
	{
		mBody->SetGravityScale(scale);
	}
}

F32 PhysicComponent::GetGravityScale() const
{
	if (IsValid())
	{
		return mBody->GetGravityScale();
	}
	else
	{
		return 1.0f;
	}
}

void PhysicComponent::SetLinearDamping(F32 value)
{
	if (IsValid())
	{
		mBody->SetLinearDamping(value);
	}
}

F32 PhysicComponent::GetLinearDamping() const
{
	if (IsValid())
	{
		return mBody->GetLinearDamping();
	}
	else
	{
		return 0.0f;
	}
}

void PhysicComponent::SetAngularDamping(F32 value)
{
	if (IsValid())
	{
		mBody->SetAngularDamping(value);
	}
}

F32 PhysicComponent::GetAngularDamping() const
{
	if (IsValid())
	{
		return mBody->GetAngularDamping();
	}
	else
	{
		return 0.0f;
	}
}

void PhysicComponent::SetFixedRotation(bool value)
{
	if (IsValid())
	{
		mBody->SetFixedRotation(value);
	}
}

bool PhysicComponent::IsFixedRotation() const
{
	if (IsValid())
	{
		return mBody->IsFixedRotation();
	}
	else
	{
		return false;
	}
}

void PhysicComponent::SetBullet(bool value)
{
	if (IsValid())
	{
		mBody->SetBullet(value);
	}
}

bool PhysicComponent::IsBullet() const
{
	if (IsValid())
	{
		return mBody->IsBullet();
	}
	else
	{
		return false;
	}
}

} // namespace en