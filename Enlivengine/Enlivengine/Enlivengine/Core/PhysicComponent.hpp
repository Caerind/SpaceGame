#pragma once

#include <Box2D/Box2D.h>

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/Math/Vector2.hpp>

#include <Enlivengine/Core/CustomTraits.hpp>
#include <Enlivengine/Core/World.hpp>

#include <Enlivengine/Core/ObjectEditor.hpp> // TODO : Should be moved elsewhere to have clear Components

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

		b2Body* GetBody() { return mBody; }
		const b2Body* GetBody() const { return mBody; }

    private:
		friend class PhysicSystem;

        b2Body* mBody;
};

} // namespace en

ENLIVE_DEFINE_TYPE_INFO(en::BodyType)

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
struct CustomObjectEditor<en::PhysicComponent>
{
	static constexpr bool value = true;
	static bool ImGuiEditor(en::PhysicComponent& object, const char* name)
	{
		bool modified = false;
		if (ImGui::CollapsingHeader(name))
		{
			ImGui::Indent();
			if (ImGui::CollapsingHeader("Body"))
			{
				ImGui::Indent();
				auto bType = object.GetBodyType();
				if (en::ObjectEditor::ImGuiEditor(bType, "BodyType"))
				{
					object.SetBodyType(bType);
					modified = true;
				}
				if (bType == en::BodyType::Dynamic || bType == en::BodyType::Kinematic)
				{
					auto linearVelocity = object.GetLinearVelocity();
					if (en::ObjectEditor::ImGuiEditor(linearVelocity, "LinearVelocity"))
					{
						object.SetLinearVelocity(linearVelocity);
						modified = true;
					}
					auto angularVelocity = object.GetAngularVelocity();
					if (en::ObjectEditor::ImGuiEditor(angularVelocity, "AngularVelocity"))
					{
						object.SetAngularVelocity(angularVelocity);
						modified = true;
					}
					auto gravityScale = object.GetGravityScale();
					if (en::ObjectEditor::ImGuiEditor(gravityScale, "GravityScale"))
					{
						object.SetGravityScale(gravityScale);
						modified = true;
					}
					auto linearDamping = object.GetLinearDamping();
					if (en::ObjectEditor::ImGuiEditor(linearDamping, "LinearDamping"))
					{
						object.SetLinearDamping(linearDamping);
						modified = true;
					}
					auto angularDamping = object.GetAngularDamping();
					if (en::ObjectEditor::ImGuiEditor(angularDamping, "AngularDamping"))
					{
						object.SetAngularDamping(angularDamping);
						modified = true;
					}
					auto fixedRotation = object.IsFixedRotation();
					if (en::ObjectEditor::ImGuiEditor(fixedRotation, "FixedRotation"))
					{
						object.SetFixedRotation(fixedRotation);
						modified = true;
					}
					auto bullet = object.IsBullet();
					if (en::ObjectEditor::ImGuiEditor(bullet, "Bullet"))
					{
						object.SetBullet(bullet);
						modified = true;
					}
				}
				ImGui::Unindent();
			}
			if (ImGui::CollapsingHeader("Fixtures"))
			{
				ImGui::Indent();
				auto* body = object.GetBody();
				// TODO : Add new fixture
				en::U32 fixtureID = 0;
				for (b2Fixture* fixture = body->GetFixtureList(); fixture; )
				{
					if (fixture != nullptr)
					{
						ImGui::Separator();
						ImGui::PushID(fixtureID);

						// TODO : Edit shape

						auto density = fixture->GetDensity();
						if (en::ObjectEditor::ImGuiEditor(density, "Density"))
						{
							fixture->SetDensity(density);
							body->ResetMassData();
							modified = true;
						}
						auto friction = fixture->GetFriction();
						if (en::ObjectEditor::ImGuiEditor(friction, "Friction"))
						{
							fixture->SetFriction(friction);
							modified = true;
						}
						auto restitution = fixture->GetRestitution();
						if (en::ObjectEditor::ImGuiEditor(restitution, "Restitution"))
						{
							fixture->SetRestitution(restitution);
							modified = true;
						}
						auto sensor = fixture->IsSensor();
						if (en::ObjectEditor::ImGuiEditor(sensor, "Sensor"))
						{
							fixture->SetSensor(sensor);
							modified = true;
						}

						if (ImGui::Button("Remove"))
						{
							b2Fixture* temp = fixture;
							fixture = fixture->GetNext();
							body->DestroyFixture(temp);
						}
						else
						{
							fixture = fixture->GetNext();
						}

						ImGui::PopID();
						fixtureID++;
					}
				}
				ImGui::Unindent();
			}
			ImGui::Unindent();
		}
		return modified;
	}
};

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