#include "Editor.h"

#include "../Core/Engine.h"

#include "../Core/Window.h"
#include "../Graphics/GraphicsEngine.h"

namespace argent::editor
{
	void Editor::OnAwake()
	{
		auto& subsystem_locator = GetEngine()->GetSubsystemLocator();

		const auto& graphics_context = subsystem_locator.Get<graphics::GraphicsEngine>()->GetGraphicsContext();

		imgui_controller_.OnAwake(subsystem_locator.Get<Window>()->GetHwnd(), 
			graphics_context.device_, graphics_context.cbv_srv_uav_heap_->PopDescriptor(), 
			graphics_context.cbv_srv_uav_heap_->GetIncrementSize(), graphics_context.cbv_srv_uav_heap_->GetGpuHandleStart());
	}

	void Editor::OnShutdown()
	{
		imgui_controller_.OnShutdown();
	}

	void Editor::Begin(const rendering::RenderContext& render_context)
	{
		imgui_controller_.Begin(render_context.GetWindowWidth(), render_context.GetWindowHeight());
	}

	void Editor::End(const rendering::RenderContext& render_context)
	{
		imgui_controller_.End(render_context.GetCommandList(), render_context.GetFrameIndex());
	}
}