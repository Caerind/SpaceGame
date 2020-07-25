#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/Window.hpp>
#include <Enlivengine/Graphics/View.hpp>
#include <Enlivengine/System/String.hpp>
#include <Enlivengine/System/ClassManager.hpp>
#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Core/ComponentManager.hpp>

#include "Components.hpp"
#include "GameState.hpp"
#include "GameSingleton.hpp"

int main(int argc, char** argv)
{
#ifdef ENLIVE_ENABLE_IMGUI
	GameSingletonTool::GetInstance().Register();
#endif // ENLIVE_ENABLE_IMGUI

	// TODO : Register engine classes in classManager & componentManager
	en::ClassManager::Register<en::NameComponent>();
	en::ClassManager::Register<en::TransformComponent>();
	en::ClassManager::Register<en::SpriteComponent>();
	en::ClassManager::Register<en::TextComponent>();
	en::ClassManager::Register<en::RenderableComponent>();
	en::ClassManager::Register<en::UIDComponent>();
	en::ComponentManager::Register<en::NameComponent>();
	en::ComponentManager::Register<en::TransformComponent>();
	en::ComponentManager::Register<en::SpriteComponent>();
	en::ComponentManager::Register<en::TextComponent>();
	en::ComponentManager::Register<en::RenderableComponent>();
	en::ComponentManager::Register<en::UIDComponent>();

	{
		// TODO : Do something about type info for pointers
		//enLogInfo(en::LogChannel::Global, "{}", en::TypeInfo<en::Array<en::U32>>::GetName());
		//enLogInfo(en::LogChannel::Global, "{}", en::TypeInfo<en::Array<en::U32*>>::GetName());

		if (argc >= 1)
		{
			en::PathManager::GetInstance().SetExecutablePath(argv[0]);
		}

		en::Application::GetInstance().Initialize();

		const std::string fileGamePath = en::PathManager::GetInstance().GetAssetsPath() + "game.xml";
		en::DataFile fileGame;
		if (fileGame.LoadFromFile(fileGamePath))
		{
			fileGame.Deserialize(GameSingleton::GetInstance(), "Game");
		}
		else
		{
			fileGame.CreateEmptyFile();
			fileGame.Serialize(GameSingleton::GetInstance(), "Game");
			fileGame.SaveToFile(fileGamePath);
		}

		en::Application& app = en::Application::GetInstance();
		app.GetWindow().create(sf::VideoMode(1024, 768), "SpaceGame", sf::Style::Titlebar | sf::Style::Close);
		app.GetWindow().getMainView().setCenter({ 512.0f, 384.0f });
		app.GetWindow().getMainView().setSize({ 1024.0f, 768.0f });
		en::PathManager::GetInstance().SetScreenshotPath("Screenshots/");

		app.Start<GameState>();
	}
	
	/*
#ifdef ENLIVE_ENABLE_DEBUG_MEMORY
	if (en::MemoryAllocator::GetDebugAllocator().GetBlockCount() > 0)
	{
		const auto& blocks = en::MemoryAllocator::GetDebugAllocator().GetBlocks();
		for (const auto& block : blocks)
		{
			enLogInfo(en::LogChannel::Global, "{} ({}): {} from {}:{}\n", block.context, block.ptr, block.size, block.GetFile().c_str(), block.line);
		}
	}
#endif // ENLIVE_ENABLE_DEBUG_MEMORY
	*/

	return 0;
}