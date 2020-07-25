#pragma once

#include <Enlivengine/System/Meta.hpp>
#include <Enlivengine/Math/Vector2.hpp>

struct VelocityComponent
{
	VelocityComponent() : velocity() {}
	VelocityComponent(const en::Vector2f& pVelocity) : velocity(pVelocity) {}

	en::Vector2f velocity;
};

