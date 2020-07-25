#pragma once

#include <Enlivengine/System/Singleton.hpp>
#include <Enlivengine/System/Meta.hpp>

#include <Enlivengine/Core/World.hpp>
#include <Enlivengine/Core/Entity.hpp>

#ifdef ENLIVE_ENABLE_IMGUI
#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Core/ObjectEditor.hpp>
#endif // ENLIVE_ENABLE_IMGUI

class GameSingleton
{
	ENLIVE_SINGLETON(GameSingleton);
	~GameSingleton() {}

public:
	en::F32 idealDistance;
	en::F32 thresholdDistance;

public:
	en::World world;
	en::Entity player1;
	en::Entity player2;
};

ENLIVE_META_CLASS_BEGIN(GameSingleton)
	ENLIVE_META_CLASS_MEMBER("idealDistance", &GameSingleton::idealDistance),
	ENLIVE_META_CLASS_MEMBER("thresholdDistance", &GameSingleton::thresholdDistance)
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
		en::ObjectEditor::ImGuiEditor(GameSingleton::GetInstance(), "GameSingleton");
	}
};
#endif // ENLIVE_ENABLE_IMGUI