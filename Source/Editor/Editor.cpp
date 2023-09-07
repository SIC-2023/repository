#include "Editor.h"

namespace argent::editor
{
	void Editor::OnAwake()
	{
		imgui_controller_ = std::make_unique<graphics::ImGuiController>();
	}

	void Editor::OnShutdown()
	{
		
	}

	void Editor::Begin(const rendering::RenderContext& render_context)
	{
		imgui_controller_->Begin(render_context.GetWindowWidth(), render_context.GetWindowHeight());
	}

	void Editor::End(const rendering::RenderContext& render_context)
	{
		imgui_controller_->End(render_context.GetCommandList(), render_context.GetFrameIndex());
	}

	void Editor::Render()
	{

	}
}