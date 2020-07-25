#include "GameSingleton.hpp"

GameSingleton::GameSingleton()
	: idealDistance(400.0f)
	, thresholdDistance(100.0f)
{
}




#ifdef ENLIVE_ENABLE_IMGUI
GameSingletonTool::GameSingletonTool() : en::ImGuiTool() {}
#endif