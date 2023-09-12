#include "SceneManager.h"
#include "../DemoScene.h"

namespace argent::scene
{
	void SceneManager::Awake()
	{
		scene_ = std::make_unique<DemoScene>();
		scene_->Awake();
		Subsystem::Awake();
	}

	void SceneManager::Shutdown()
	{
		scene_->Shutdown();
		Subsystem::Shutdown();
	}

	void SceneManager::Update()
	{
		scene_->Update();
	}

	void SceneManager::Render(const rendering::RenderContext& render_context)
	{
		scene_->Render(render_context);
	}

}