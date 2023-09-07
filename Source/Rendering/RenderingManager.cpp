#include "RenderingManager.h"

#include "RenderContext.h"
#include "../Scene/BaseScene.h"

namespace argent::rendering
{
	RenderingManager::RenderingManager()
	{

	}

	RenderingManager::~RenderingManager()
	{

	}

	void RenderingManager::OnAwake()
	{
		Subsystem::OnAwake();
	}

	void RenderingManager::OnShutdown()
	{
		
	}

	void RenderingManager::Execute(const RenderContext& render_context, scene::BaseScene* scene)
	{
		scene->Render(render_context);
	}
}
