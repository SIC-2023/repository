#include "Engine.h"

#include <iostream>

#include "../Utility/Misc.h"

#include "../Core/Window.h"
#include "../Core/Timer.h"

#include "../Input/InputManager.h"

#include "../Graphics/ImGuiController.h"
#include "../Graphics/FrameResource.h"
#include "../Graphics/GraphicsEngine.h"

#include "../Rendering/RenderContext.h"
#include "../Rendering/RenderingManager.h"

#include "../Editor/Editor.h"

#include "../Scene/SceneManager.h"

namespace argent
{
	Engine* engine = nullptr;

	Engine* GetEngine()
	{
		return engine;
	}

	Engine::Engine(bool is_editor_mode):
		is_request_showdown_(false)
	,	is_editor_mode_(is_editor_mode)
	{
		if (engine) _ASSERT_EXPR(FALSE, L"Already Instantiated : argent::Engine");

		engine = this;

		std::cout << "Engine are instantiated" << std::endl;	
	}

	Engine::~Engine()
	{
				
	}

	void Engine::SetUp()
	{
		//add various subsystem on subsystem locator
		subsystem_locator_.Add<Window>();

		subsystem_locator_.Add<input::InputManager>();

		subsystem_locator_.Add<graphics::GraphicsEngine>();

		subsystem_locator_.Add<rendering::RenderingManager>();

		if(is_editor_mode_) subsystem_locator_.Add<editor::Editor>();

		subsystem_locator_.Add<scene::SceneManager>();

		subsystem_locator_.Add<Timer>();
	}

	void Engine::OnShutdown()
	{
		//remove various subsystem from subsystem locator
		subsystem_locator_.Remove<Timer>();

		subsystem_locator_.Remove<scene::SceneManager>();

		if(is_editor_mode_) subsystem_locator_.Remove<editor::Editor>();

		subsystem_locator_.Remove<rendering::RenderingManager>();

		subsystem_locator_.Remove<graphics::GraphicsEngine>();

		subsystem_locator_.Remove<input::InputManager>();

		subsystem_locator_.Remove<Window>();
	}

	void Engine::Run()
	{
		//update and rendering

		//Update window event and message
		const auto& window = subsystem_locator_.Get<Window>();
		window->ProcessSystemEventQueue();
		

		//Update input (keyboard, mouse and Gamepad)
		subsystem_locator_.Get<input::InputManager>()->Update(window->GetHwnd());

		//Update timer (delta time and total time)
		subsystem_locator_.Get<Timer>()->Tick();

		//Update scene object(like player, enemy, camera ...) on cpu side
		const auto scene_manager = subsystem_locator_.Get<scene::SceneManager>();

		if(!GetIsEditorMode() || 
			(subsystem_locator_.Get<editor::Editor>()->GetIsPlay() && 
				!subsystem_locator_.Get<editor::Editor>()->GetIsPause()))
		{
			scene_manager->Update();
		}

		//Draw scene
		const auto gfx = subsystem_locator_.Get<graphics::GraphicsEngine>();
		const auto render_context = gfx->Begin();

		subsystem_locator_.Get<rendering::RenderingManager>()->Execute(render_context, scene_manager->GetCurrentScene());

		gfx->End(render_context);
	}
}
