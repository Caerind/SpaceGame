#pragma once

#include <Box2D/Box2D.h>
#include <SFML/Graphics/RenderTarget.hpp>

#include <Enlivengine/System/Time.hpp>
#include <Enlivengine/Math/Vector2.hpp>

namespace en
{

class World;
class Entity;
class PhysicComponent;

class PhysicSystem : public b2Draw, public b2ContactListener
{
    public:
        PhysicSystem(World& world);
        ~PhysicSystem();

		bool Initialize(const Entity& entity, PhysicComponent& component);
        
        void Update(Time dt);

		void Play();
		void Pause();
		bool IsPlaying() const;
        
        void DisableGravity();
        void SetGravity(const Vector2f& gravity);
        Vector2f GetGravity() const;
        
        void SetPixelsPerMeter(F32 value);
        F32 GetPixelsPerMeter() const;
        
        void SetVelocityIterations(U32 value);
        U32 GetVelocityIterations() const;
        void SetPositionIterations(U32 value);
        U32 GetPositionIterations() const;

#ifdef ENLIVE_DEBUG
		void Render(sf::RenderTarget& target);

		void SetDebugRendering(bool value);
		bool IsDebugRendering() const;

		void SetDebugRenderFlags(U32 flags);
		U32 GetDebugRenderFlags() const;

		void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
		void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
		void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
		void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
		void DrawTransform(const b2Transform& xf);
		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);
#endif // ENLIVE_DEBUG

        // TODO : ContactListener

    protected:
        World& mWorld;
		b2World* mPhysicWorld;
        F32 mPixelsPerMeter;
        U32 mVelocityIterations;
		U32 mPositionIterations;
		bool mPlaying;

#ifdef ENLIVE_DEBUG
		sf::RenderTarget* mDebugRenderTarget;
		U32 mDebugRenderFlags;
        bool mDebugRender;
#endif // ENLIVE_DEBUG
};
        
} // namespace en