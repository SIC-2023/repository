#include "SceneManager.h"
#include "../DemoScene.h"

namespace argent::scene
{
	void SceneManager::OnAwake()
	{
		scene_ = std::make_unique<DemoScene>();
		scene_->OnAwake();
		Subsystem::OnAwake();
	}

	void SceneManager::OnShutdown()
	{
		scene_->OnShutdown();
		Subsystem::OnShutdown();
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