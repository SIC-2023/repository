#include "Engine.h"

#include <iostream>

#include "../Utility/Misc.h"
#include "../Core/Window.h"
#include "../Core/Timer.h"
#include "../Input/InputManager.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Scene/SceneManager.h"

namespace argent
{
	Engine* engine = nullptr;

	Engine* GetEngine()
	{
		return engine;
	}

	Engine::Engine():
		is_request_showdown_(false)
	{
		if (engine) _ASSERT_EXPR(FALSE, L"エンジンのインスタンスが複数定義されています");

		engine = this;

		std::cout << "エンジンが作成されました" << std::endl;	
	}

	Engine::~Engine()
	{
		//TODO subsystemLocatorにRemoveを作成する
		
	}

	void Engine::SetUp()
	{
		//TODO 各種サブシステムを追加していく
		subsystem_locator_.Add<Window>();

		subsystem_locator_.Add<input::InputManager>();

		subsystem_locator_.Add<graphics::GraphicsEngine>();
		subsystem_locator_.Add<scene::SceneManager>();
		subsystem_locator_.Add<Timer>();
	}

	void Engine::OnShutdown()
	{
		//TODO
		subsystem_locator_.Remove<Timer>();
		subsystem_locator_.Remove<scene::SceneManager>();
		subsystem_locator_.Remove<graphics::GraphicsEngine>();

		subsystem_locator_.Remove<input::InputManager>();

		subsystem_locator_.Remove<Window>();
	}

	void Engine::Run()
	{
		//TODO シーンマネージャ読んだりとか

		//ウィンドウイベント、メッセージの更新
		const auto& window = subsystem_locator_.Get<Window>();
		window->ProcessSystemEventQueue();
		

		//キー入力の更新
		subsystem_locator_.Get<input::InputManager>()->Update(window->GetHwnd());

		//タイマー更新
		subsystem_locator_.Get<Timer>()->Tick();

		//シーンアップデート
		const auto scene_manager = subsystem_locator_.Get<scene::SceneManager>();
		scene_manager->Update();

		//描画
		const auto gfx = subsystem_locator_.Get<graphics::GraphicsEngine>();

		const auto render_context = gfx->Begin();

		scene_manager->Render(render_context);

		gfx->End(render_context);
	}
}
