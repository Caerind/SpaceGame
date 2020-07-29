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

struct AIParams
{
	AIParams()
		: timeRecompMin(en::Time::Seconds(0.3f))
		, timeRecompMax(en::Time::Seconds(1.0f))
		, cooldown(en::Time::Seconds(0.3f))
		, cooldownDev(en::Time::Seconds(0.1f))
		, randDev(15.0f)
		, evF(1.2f)
		, evD(400.0f)
		, alF(1.0f)
		, plF(1.0f)
		, rnF(1.0f)
	{}

	en::Time timeRecompMin;
	en::Time timeRecompMax;
	en::Time cooldown;
	en::Time cooldownDev;
	en::F32 randDev;
	en::F32 evF;
	en::F32 evD;
	en::F32 alF;
	en::F32 plF;
	en::F32 rnF;
	en::F32 plD;
	en::F32 plI;
};
ENLIVE_META_CLASS_BEGIN(AIParams)
ENLIVE_META_CLASS_MEMBER("timeRecompMin", &AIParams::timeRecompMin),
ENLIVE_META_CLASS_MEMBER("timeRecompMax", &AIParams::timeRecompMax),
ENLIVE_META_CLASS_MEMBER("cooldown", &AIParams::cooldown),
ENLIVE_META_CLASS_MEMBER("cooldownDev", &AIParams::cooldownDev),
ENLIVE_META_CLASS_MEMBER("randDev", &AIParams::randDev),
ENLIVE_META_CLASS_MEMBER("evF", &AIParams::evF),
ENLIVE_META_CLASS_MEMBER("evD", &AIParams::evD),
ENLIVE_META_CLASS_MEMBER("alF", &AIParams::alF),
ENLIVE_META_CLASS_MEMBER("plF", &AIParams::plF),
ENLIVE_META_CLASS_MEMBER("rnF", &AIParams::rnF),
ENLIVE_META_CLASS_MEMBER("plD", &AIParams::plD),
ENLIVE_META_CLASS_MEMBER("plI", &AIParams::plI)
ENLIVE_META_CLASS_END()

struct BodyPart
{
	en::Recti body;
	en::Recti bodyColor;
	std::array<en::Recti, 4> damages;
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
	en::F32 degPerSecond;
	std::array<BodyPart, 8> bodys;
	std::array<WingsPart, 8> wings;
	std::array<CanonPart, 8> canons;
	std::array<EnginePart, 5> engines;
	std::array<ArmPart, 5> arms;
	AIParams aiParams;

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
	ENLIVE_META_CLASS_MEMBER("playerForce", &GameSingleton::playerForce),
	ENLIVE_META_CLASS_MEMBER("degPerSecond", &GameSingleton::degPerSecond),
	ENLIVE_META_CLASS_MEMBER("bodys", &GameSingleton::bodys),
	ENLIVE_META_CLASS_MEMBER("wings", &GameSingleton::wings),
	ENLIVE_META_CLASS_MEMBER("canons", &GameSingleton::canons),
	ENLIVE_META_CLASS_MEMBER("engines", &GameSingleton::engines),
	ENLIVE_META_CLASS_MEMBER("arms", &GameSingleton::arms),
	ENLIVE_META_CLASS_MEMBER("aiParams", &GameSingleton::aiParams)
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

		if (gameSing.world.IsPlaying())
		{
			if (ImGui::Button("PauseWorld"))
			{
				gameSing.world.Pause();
			}
		}
		else
		{
			if (ImGui::Button("PlayWorld"))
			{
				gameSing.world.Play();
			}
		}

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