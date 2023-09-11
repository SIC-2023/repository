#include "PostProcessManager.h"


namespace argent::rendering
{
	void PostProcessManager::OnAwake(const graphics::GraphicsContext& graphics_context)
	{

	}

	void PostProcessManager::OnShutdown()
	{
		
	}


	void PostProcessManager::Execute(const RenderContext& render_context, uint32_t scene_srv_heap_index)
	{
		result_srv_heap_index_ = scene_srv_heap_index;
	}

}
