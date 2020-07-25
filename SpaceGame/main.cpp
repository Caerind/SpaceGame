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
#include "AudioTrackMixer3000.hpp"

int main(int argc, char** argv)
{
#ifdef ENLIVE_ENABLE_IMGUI
	GameSingletonTool::GetInstance().Register();
	AudioTrackMixer3000Tool::GetInstance().Register();
#endif // ENLIVE_ENABLE_IMGUI

	//en::ClassManager::Register<TrackData>();
	//en::ClassManager::Register<AudioTrackMixer3000>();
	//en::ClassManager::Register<GameSingleton>();

	// TODO : Register engine classes in classManager & componentManager
	en::ClassManager::Register<en::NameComponent>();
	en::ClassManager::Register<en::TransformComponent>();
	en::ClassManager::Register<en::SpriteComponent>();
	en::ClassManager::Register<en::TextComponent>();
	en::ClassManager::Register<en::RenderableComponent>();
	en::ClassManager::Register<en::UIDComponent>();
	en::ClassManager::Register<VelocityComponent>();
	en::ClassManager::Register<ShipComponent>();

	// TODO : Register engine classes in classManager & componentManager
	en::ComponentManager::Register<en::NameComponent>();
	en::ComponentManager::Register<en::TransformComponent>();
	en::ComponentManager::Register<en::SpriteComponent>();
	en::ComponentManager::Register<en::TextComponent>();
	en::ComponentManager::Register<en::RenderableComponent>();
	en::ComponentManager::Register<en::UIDComponent>();
	en::ComponentManager::Register<VelocityComponent>();
	en::ComponentManager::Register<ShipComponent>();

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
			fileGame.Deserialize(AudioTrackMixer3000::GetInstance(), "Audio");
		}
		else
		{
			fileGame.CreateEmptyFile();
			fileGame.Serialize(GameSingleton::GetInstance(), "Game");
			fileGame.Serialize(AudioTrackMixer3000::GetInstance(), "Audio");
			fileGame.SaveToFile(fileGamePath);
		}

		en::Application& app = en::Application::GetInstance();
		app.GetWindow().create(sf::VideoMode(1024, 768), "SpaceGame", sf::Style::Titlebar | sf::Style::Close);
		app.GetWindow().getMainView().setCenter({ 512.0f, 384.0f });
		app.GetWindow().getMainView().setSize({ 1024.0f, 768.0f });
		en::PathManager::GetInstance().SetScreenshotPath("Screenshots/");

		auto& actionSystem = app.GetActionSystem();
		actionSystem.AddInputJoystickConnect("player1JoystickConnected", 0, en::ActionType::Hold);
		actionSystem.AddInputJoystickConnect("player2JoystickConnected", 1, en::ActionType::Hold);
		actionSystem.AddInputJoystickConnect("player1JoystickConnect", 0, en::ActionType::Pressed);
		actionSystem.AddInputJoystickConnect("player2JoystickConnect", 1, en::ActionType::Pressed);
		actionSystem.AddInputJoystickConnect("player1JoystickDisconnect", 0, en::ActionType::Released);
		actionSystem.AddInputJoystickConnect("player2JoystickDisconnect", 1, en::ActionType::Released);
		actionSystem.AddInputJoystickButton("player1JoystickButtonA", 0, 0, en::ActionType::Pressed);
		actionSystem.AddInputJoystickButton("player2JoystickButtonA", 1, 0, en::ActionType::Pressed);

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