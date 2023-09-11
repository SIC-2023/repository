#include "HierarchyWindow.h"

#include "imgui.h"

namespace argent::editor
{
	HierarchyWindow::HierarchyWindow():
		EditorWindow("Hierarchy")
	{
	}

	void HierarchyWindow::OnAwake()
	{
		EditorWindow::OnAwake();
	}

	void HierarchyWindow::OnShutdown()
	{
		EditorWindow::OnShutdown();
	}

	void HierarchyWindow::OnRender()
	{
		ImGui::SetNextWindowSize(ImVec2(280, 280), ImGuiCond_FirstUseEver);
		ImGui::Begin(GetName().c_str());

		ImGui::End();
	}
}