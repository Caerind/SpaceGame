#include "GameSingleton.hpp"

GameSingleton::GameSingleton()
	: distanceMaxInitial(1000.0f)
	, distanceMaxPerSecond(10.0f)
	, idealDistancePlayer(600.0f)
	, thresholdDistancePlayer(100.0f)
	, startPosition()
	, startOffsetPlayer1()
	, startOffsetPlayer2()
	, startRotationPlayer1(0.0f)
	, startRotationPlayer2(0.0f)
	, energyFactorAI(0.9f)
	, degPerSecond(10.0f)
	, world()
	, player1()
	, player2()
	, distanceMax(distanceMaxInitial)
	, posCenter()
	, energyFactor(1.0f)
	, energyFactorEnv(1.0f)
{
}

en::Entity GameSingleton::GetPlayer(en::U32 playerIndex) const
{
	if (playerIndex == 0)
		return player1;
	else if (playerIndex == 1)
		return player2;
	else
		enAssert(false);
	return player1;
}



#ifdef ENLIVE_ENABLE_IMGUI
GameSingletonTool::GameSingletonTool() : en::ImGuiTool() {}
#endif