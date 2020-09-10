#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/Window.hpp>
#include <Enlivengine/Graphics/View.hpp>
#include <Enlivengine/System/String.hpp>
#include <Enlivengine/System/ClassManager.hpp>
#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Core/PhysicComponent.hpp>
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
	en::ClassManager::Register<en::PhysicComponent>();
	en::ClassManager::Register<VelocityComponent>();
	en::ClassManager::Register<ShipComponent>();
	en::ClassManager::Register<ShootComponent>();
	en::ClassManager::Register<UIComponent>();
	en::ClassManager::Register<BarkComponent>();
	en::ClassManager::Register<AIComponent>();
	en::ClassManager::Register<PlayerComponent>();
	en::ClassManager::Register<PlanetComponent>();
	en::ClassManager::Register<SpacePhysicComponent>();

	// TODO : Register engine classes in classManager & componentManager
	en::ComponentManager::Register<en::NameComponent>();
	en::ComponentManager::Register<en::TransformComponent>();
	en::ComponentManager::Register<en::SpriteComponent>();
	en::ComponentManager::Register<en::TextComponent>();
	en::ComponentManager::Register<en::RenderableComponent>();
	en::ComponentManager::Register<en::UIDComponent>();
	en::ComponentManager::Register<en::PhysicComponent>();
	en::ComponentManager::Register<VelocityComponent>();
	en::ComponentManager::Register<ShipComponent>();
	en::ComponentManager::Register<ShootComponent>();
	en::ComponentManager::Register<UIComponent>();
	en::ComponentManager::Register<BarkComponent>();
	en::ComponentManager::Register<AIComponent>();
	en::ComponentManager::Register<PlayerComponent>();
	en::ComponentManager::Register<PlanetComponent>();
	en::ComponentManager::Register<SpacePhysicComponent>();

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
		sf::VideoMode vm = sf::VideoMode::getDesktopMode();
		app.GetWindow().create(vm, "SpaceGame", sf::Style::Titlebar | sf::Style::Close);
		app.GetWindow().getMainView().setSize({ vm.width * 1.0f, vm.height * 1.0f });
		app.GetWindow().getMainView().setCenter({ vm.width * 0.5f, vm.height * 0.5f });
		app.GetWindow().setKeyRepeatEnabled(false);
		//app.GetWindow().toFullscreen();
		en::PathManager::GetInstance().SetScreenshotPath("Screenshots/");

		auto& actionSystem = app.GetActionSystem();
		actionSystem.AddInputJoystickButton("player1JoystickFire", 0, 0, en::ActionType::Pressed);
		actionSystem.AddInputJoystickButton("player2JoystickFire", 1, 0, en::ActionType::Pressed);
		actionSystem.AddInputKey("player1KeyFire", sf::Keyboard::Space, en::ActionType::Pressed);
		actionSystem.AddInputKey("player2KeyFire", sf::Keyboard::Enter, en::ActionType::Pressed);
		actionSystem.AddInputOr("player1Fire", actionSystem.GetInputByName("player1JoystickFire")->GetID(), actionSystem.GetInputByName("player1KeyFire")->GetID());
		actionSystem.AddInputOr("player2Fire", actionSystem.GetInputByName("player2JoystickFire")->GetID(), actionSystem.GetInputByName("player2KeyFire")->GetID());

#ifdef ENLIVE_DEBUG
		actionSystem.AddInputKey("save", sf::Keyboard::S, en::ActionType::Pressed, static_cast<en::U32>(en::ActionInputKey::KeyCombination::Ctrl));
		actionSystem.AddInputMouse("rightMouseButton", sf::Mouse::Button::Right, en::ActionType::Pressed);
		actionSystem.AddInputMouse("middleMouseButton", sf::Mouse::Button::Middle, en::ActionType::Pressed);
#endif // ENLIVE_DEBUG

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