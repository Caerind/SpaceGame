#include "GameSingleton.hpp"

GameSingleton::GameSingleton()
	: someSpeed(400.0f)
{
}




#ifdef ENLIVE_ENABLE_IMGUI
GameSingletonTool::GameSingletonTool() : en::ImGuiTool() {}
#endif