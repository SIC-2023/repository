#include "Editor.h"

#include "imgui.h"
#include "imgui_internal.h"

#include "../Core/Engine.h"
#include "../Core/Window.h"

#include "SceneWindow.h"
#include "InspectorWindow.h"
#include "HierarchyWindow.h"
#include "ProjectWindow.h"

#include "../Graphics/GraphicsEngine.h"
#include "../Graphics/Texture.h"

#include "../Scene/SceneManager.h"

namespace argent::editor
{
	uint64_t Editor::scene_srv_heap_index_ = 0;

	void Editor::Awake()
	{
		auto& subsystem_locator = GetEngine()->GetSubsystemLocator();

		const auto& graphics_context = subsystem_locator.Get<graphics::GraphicsEngine>()->GetGraphicsContext();

		imgui_controller_.Awake(subsystem_locator.Get<Window>()->GetHwnd(), 
			graphics_context.device_, graphics_context.cbv_srv_uav_heap_->PopDescriptor(), 
			graphics_context.cbv_srv_uav_heap_->GetIncrementSize(), graphics_context.cbv_srv_uav_heap_->GetGpuHandleStart());

		//Register Editor Window
		Register<SceneWindow>();
		Register<HierarchyWindow>();
		Register<InspectorWindow>();
		Register<ProjectWindow>();

		play_texture_ = std::make_unique<graphics::Texture>(graphics_context, L"./Assets/Engine/Play.png");
		pause_texture_ = std::make_unique<graphics::Texture>(graphics_context, L"./Assets/Engine/Pause.png");
	}

	void Editor::Shutdown()
	{
		imgui_controller_.OnShutdown();
	}

	void Editor::OnRender(const rendering::RenderContext& render_context, uint64_t scene_srv_heap_index)
	{
		scene_srv_heap_index_ = scene_srv_heap_index;

		imgui_controller_.Begin(render_context.GetWindowWidth(), render_context.GetWindowHeight());

		ImGuiID docking_id{};
		//Generate Null DockSpace
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

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

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace Demo", &is_open, window_flags);
			ImGui::PopStyleVar();

			ImGui::PopStyleVar(2);


			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 8.0f));
			const bool is_open_menu_bar = ImGui::BeginMainMenuBar();
			ImGui::PopStyleVar();
			if(is_open_menu_bar)
			{
				static ImVec4 play_color{ 1, 1, 1, 1 };
				static ImVec4 pause_color{ 1, 1, 1, 1 };
				constexpr ImVec2 size = { 25, 25 };
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - size.x / 2);
				if(ImGui::ImageButton("Play", reinterpret_cast<ImTextureID>(play_texture_->GetHeapIndex()),
					size, ImVec2(), ImVec2(1, 1),
					ImVec4(0, 0, 0, 0), play_color))
				{
					if(is_play_ && is_pause_)
					{
						is_pause_ = false;
						pause_color = { 1, 1, 1, 1 };
					}
					else if(is_play_)
					{
						is_play_ = is_pause_ = false;
						play_color = { 1, 1, 1, 1 };
						pause_color = { 1, 1, 1, 1 };
					}
					else
					{
						is_play_ = true;
						is_pause_ = false;
						play_color = { 0.1f, 0.5f, 1.0f, 1.0f };
					}
				}

				if(ImGui::ImageButton("Pause", reinterpret_cast<ImTextureID>(pause_texture_->GetHeapIndex()),
					size, ImVec2(), ImVec2(1, 1),
					ImVec4(0, 0, 0, 0), pause_color))
				{
					if(is_play_)
					{
						is_pause_ = true;
						pause_color = { 0.1f, 0.5f, 1.0f, 1.0f };
					}
				}

				ImGui::EndMainMenuBar();
			}

			//ドッキングレイアウトの作成
			static bool first_loop = true;
			if(first_loop)
			{
				//新しくドックレイアウトを作るのにワークスペースの幅高さ、中心が必要
				const ImVec2 work_pos = ImGui::GetMainViewport()->WorkPos;
				const ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
				const ImVec2 work_center{ work_pos.x + work_size.x, work_pos.y + work_size.y };

				ImGuiID id = ImGui::GetID("OriginalDockSpace");	//任意の文字列で取得
				ImGui::DockBuilderRemoveNode(id);	//すでに存在するレイアウトを削除
				ImGui::DockBuilderAddNode(id);

				ImVec2 size{ render_context.GetWindowWidth(), render_context.GetWindowHeight() }; //多分任意の値　

				ImVec2 node_pos{ work_center.x - size.x * 0.5f, work_center.y - size.y * 0.5f };

				ImGui::DockBuilderSetNodeSize(id, size);
				ImGui::DockBuilderSetNodePos(id, node_pos);

				//ノードの分割
				ImGuiID scene_dock_id = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.6f, nullptr, &id);
				ImGuiID hierarchy_dock_id = ImGui::DockBuilderSplitNode(id, ImGuiDir_Left, 0.5f, nullptr, &id);
				ImGuiID project_dock_id = ImGui::DockBuilderSplitNode(hierarchy_dock_id, ImGuiDir_Down, 0.5f, nullptr, &hierarchy_dock_id);
				ImGuiID inspector_dock_id = ImGui::DockBuilderSplitNode(id, ImGuiDir_Right, 0.5f, nullptr, &id);
				ImGuiID console_dock_id = ImGui::DockBuilderSplitNode(scene_dock_id, ImGuiDir_Down, 0.3f, nullptr	, &scene_dock_id);

				ImGui::DockBuilderDockWindow("Scene", scene_dock_id);
				ImGui::DockBuilderDockWindow("Project", project_dock_id);
				ImGui::DockBuilderDockWindow("Hierarchy", hierarchy_dock_id);
				ImGui::DockBuilderDockWindow("Inspector", inspector_dock_id);
				ImGui::DockBuilderDockWindow("Console", console_dock_id);

				ImGui::DockBuilderFinish(id);

				first_loop = false;
			}

			ImGuiIO& io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				const ImGuiID dockspace_id = ImGui::GetID("OriginalDockSpace");
				docking_id = ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}
			ImGui::End();
		}

		for(auto& editor_window : editor_windows_)
		{
			editor_window->OnRender();
		}

		ImGui::Begin("Console");
		ImGui::End();
		
		imgui_controller_.End(render_context.GetCommandList(), render_context.GetFrameIndex());
	}
}