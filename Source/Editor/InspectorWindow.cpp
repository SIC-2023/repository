#include "InspectorWindow.h"

#include "imgui.h"

#include "../GameObject/GameObject.h"

namespace argent::editor
{
	GameObject* InspectorWindow::focused_game_object_ = nullptr;
	InspectorWindow::InspectorWindow():
		EditorWindow("Inspector")
	{
	}

	void InspectorWindow::OnAwake()
	{
		EditorWindow::OnAwake();
	}

	void InspectorWindow::OnShutdown()
	{
		EditorWindow::OnShutdown();
	}

	void InspectorWindow::OnRender()
	{
		ImGui::SetNextWindowSize(ImVec2(280, 280), ImGuiCond_FirstUseEver);
		ImGui::Begin(GetName().c_str());

		if(focused_game_object_)
		{
			focused_game_object_->OnDrawInspector();
		}
		ImGui::End();
	}
}
