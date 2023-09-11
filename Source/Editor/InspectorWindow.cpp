#include "InspectorWindow.h"

#include "imgui.h"

namespace argent::editor
{
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

		ImGui::End();
	}
}