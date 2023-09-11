#include "Editor.h"

#include "../Core/Engine.h"

#include "../Core/Window.h"
#include "../Graphics/GraphicsEngine.h"

#include "../Scene/SceneManager.h"

#include "imgui.h"
#include "imgui_internal.h"

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

	void Editor::OnRender(const rendering::RenderContext& render_context, uint64_t scene_srv_heap_index)
	{
		imgui_controller_.Begin(render_context.GetWindowWidth(), render_context.GetWindowHeight());

		ImGuiID docking_id{};
		//Generate Null DockSpace
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;


			static bool opt_padding = false;
			static bool is_open = true;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

			if (!opt_padding)
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &is_open, window_flags);
			if (!opt_padding)
				ImGui::PopStyleVar();

			ImGui::PopStyleVar(2);

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				docking_id = ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			ImGui::End();
		}

		ImGui::SetNextWindowDockID(docking_id, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(280, 280), ImGuiCond_FirstUseEver);
		ImGui::Begin("Scene");
		GetEngine()->GetSubsystemLocator().Get<scene::SceneManager>()->GetCurrentScene()->DrawGui();

		//TODO ÀÛ‚Ì‰æ–ÊƒTƒCƒY‚©‚ç•‚‚³‚ğ’²ß‚·‚é
		float imgui_window_width = ImGui::GetWindowWidth();
		ImGui::Image(reinterpret_cast<ImTextureID>(scene_srv_heap_index), ImVec2(imgui_window_width, imgui_window_width / 1280.0f * 720.0f));
		ImGui::End();
		
		imgui_controller_.End(render_context.GetCommandList(), render_context.GetFrameIndex());
	}
}