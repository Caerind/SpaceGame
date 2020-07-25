#pragma once

#include <Enlivengine/System/Singleton.hpp>
#include <Enlivengine/System/Meta.hpp>

#ifdef ENLIVE_ENABLE_IMGUI
#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Core/ObjectEditor.hpp>
#endif // ENLIVE_ENABLE_IMGUI

class GameSingleton
{
	ENLIVE_SINGLETON(GameSingleton);
	~GameSingleton() {}

public:
	en::F32 someSpeed;
};

ENLIVE_META_CLASS_BEGIN(GameSingleton)
	ENLIVE_META_CLASS_MEMBER("someSpeed", &GameSingleton::someSpeed)
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