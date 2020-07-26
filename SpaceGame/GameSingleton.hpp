#pragma once

#include <Enlivengine/System/Singleton.hpp>
#include <Enlivengine/System/Meta.hpp>

#include <Enlivengine/Core/World.hpp>
#include <Enlivengine/Core/Entity.hpp>
#include <Enlivengine/Core/Components.hpp>
#include "Components.hpp"

#ifdef ENLIVE_ENABLE_IMGUI
#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Core/ObjectEditor.hpp>
#endif // ENLIVE_ENABLE_IMGUI

class GameSingleton
{
	ENLIVE_SINGLETON(GameSingleton);
	~GameSingleton() {}

public:
	en::F32 distanceMaxInitial;
	en::F32 distanceMaxPerSecond;
	en::F32 idealDistancePlayer;
	en::F32 thresholdDistancePlayer;
	en::Vector2f startPosition;
	en::Vector2f startOffsetPlayer1;
	en::Vector2f startOffsetPlayer2;
	en::F32 startRotationPlayer1;
	en::F32 startRotationPlayer2;
	en::F32 energyFactorAI;
	en::F32 playerForce;

public:
	en::World world;
	en::Entity player1;
	en::Entity player2;

	en::F32 distanceMax;
	en::Vector2f posCenter;
	en::F32 playerDelta;
	en::F32 energyFactor;
	en::F32 energyFactorEnv;
	en::Vector2f playerForceVector;

#ifdef ENLIVE_DEBUG
	en::Vector2f player1Velocity;
	en::Vector2f player2Velocity;
#endif // ENLIVE_DEBUG

	void UpdateStats(en::Time dt)
	{
		distanceMax += dt.AsSeconds() * distanceMaxPerSecond;

		en::Vector2f p1Pos = player1.Get<en::TransformComponent>().transform.GetPosition2D();
		en::Vector2f p2Pos = player2.Get<en::TransformComponent>().transform.GetPosition2D();
		en::Vector2f delta = p2Pos - p1Pos;
		en::F32 playerDistanceSqr = delta.getSquaredLength();
		playerDelta = delta.getLength();
		posCenter = (p1Pos + p2Pos) * 0.5f;
		const en::F32 startMaxThreshold = (idealDistancePlayer * 2) - thresholdDistancePlayer;
		const en::F32 startMaxThresholdSqr = startMaxThreshold * startMaxThreshold;
		const en::F32 startMinThreshold = thresholdDistancePlayer;
		const en::F32 startMinThresholdSqr = startMinThreshold * startMinThreshold;
		en::F32 playerForceFactor = 0.0f;
		if (playerDistanceSqr > startMaxThreshold * startMaxThreshold)
		{
			playerForceFactor = playerForce;
			if (playerDistanceSqr > startMaxThresholdSqr * 2.0f)
			{
				playerForceFactor *= 2.0f * energyFactorEnv;
			}
		}
		else if (playerDistanceSqr < startMinThresholdSqr)
		{
			playerForceFactor = -playerForce;
			if (playerDistanceSqr < startMinThresholdSqr * 2.0f)
			{
				playerForceFactor *= 2.0f * energyFactorEnv;
			}
		}
		playerForceVector = playerForceFactor * delta.normalized();

		world.GetFreeCamView().setCenter(posCenter);
		world.GetGameView().setCenter(posCenter);

		if (posCenter.getSquaredLength() < 1.0f)
		{
			energyFactor = 1.0f;
		}
		else
		{
			energyFactor = energyFactorEnv * ((-posCenter.getLength()/ distanceMax) + 2.0f);
		}
	}

	en::Entity GetPlayer(en::U32 playerIndex) const;
};

ENLIVE_META_CLASS_BEGIN(GameSingleton)
	ENLIVE_META_CLASS_MEMBER("distanceMaxInitial", &GameSingleton::distanceMaxInitial),
	ENLIVE_META_CLASS_MEMBER("distanceMaxPerSecond", &GameSingleton::distanceMaxPerSecond),
	ENLIVE_META_CLASS_MEMBER("idealDistancePlayer", &GameSingleton::idealDistancePlayer),
	ENLIVE_META_CLASS_MEMBER("thresholdDistancePlayer", &GameSingleton::thresholdDistancePlayer),
	ENLIVE_META_CLASS_MEMBER("startPosition", &GameSingleton::startPosition),
	ENLIVE_META_CLASS_MEMBER("startOffsetPlayer1", &GameSingleton::startOffsetPlayer1),
	ENLIVE_META_CLASS_MEMBER("startOffsetPlayer2", &GameSingleton::startOffsetPlayer2),
	ENLIVE_META_CLASS_MEMBER("startRotationPlayer1", &GameSingleton::startRotationPlayer1),
	ENLIVE_META_CLASS_MEMBER("startRotationPlayer2", &GameSingleton::startRotationPlayer2),
	ENLIVE_META_CLASS_MEMBER("energyFactorAI", &GameSingleton::energyFactorAI),
	ENLIVE_META_CLASS_MEMBER("playerForce", &GameSingleton::playerForce)
ENLIVE_META_CLASS_END()



#ifdef ENLIVE_ENABLE_IMGUI
class GameSingletonTool : public en::ImGuiTool
{
	ENLIVE_SINGLETON(GameSingletonTool);
	~GameSingletonTool() {}

public:
	virtual en::ImGuiToolTab GetTab() const { return en::ImGuiToolTab::Game; }
	virtual const char* GetName() const { return "GameConfig"; }

	virtual void Display() 
	{
		auto& gameSing = GameSingleton::GetInstance();

		en::ObjectEditor::ImGuiEditor(gameSing, "GameSingleton");

		en::ObjectEditor::ImGuiEditor(gameSing.distanceMax, "distanceMax");
		en::ObjectEditor::ImGuiEditor(gameSing.posCenter, "posCenter");
		en::ObjectEditor::ImGuiEditor(gameSing.playerDelta, "playerDelta");
		en::ObjectEditor::ImGuiEditor(gameSing.energyFactor, "energyFactor");
		en::ObjectEditor::ImGuiEditor(gameSing.energyFactorEnv, "energyFactorEnv");
		en::ObjectEditor::ImGuiEditor(gameSing.playerForceVector, "playerForceVector");

#ifdef ENLIVE_DEBUG
		en::ObjectEditor::ImGuiEditor(gameSing.player1Velocity, "player1Velocity");
		en::ObjectEditor::ImGuiEditor(gameSing.player2Velocity, "player2Velocity");
#endif // ENLIVE_DEBUG

	}
};
#endif // ENLIVE_ENABLE_IMGUI