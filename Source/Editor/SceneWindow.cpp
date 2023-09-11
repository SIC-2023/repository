#include "SceneWindow.h"

#include "imgui.h"

namespace argent::editor
{
	SceneWindow::SceneWindow():
		EditorWindow("SceneWindow")
	{
	}

	void SceneWindow::OnAwake()
	{
		EditorWindow::OnAwake();
	}

	void SceneWindow::OnShutdown()
	{
		EditorWindow::OnShutdown();
	}

	void SceneWindow::OnRender()
	{
		ImGui::SetNextWindowSize(ImVec2(280, 280), ImGuiCond_FirstUseEver);
		ImGui::Begin(GetName().c_str());

		ImGui::End();
	}

}
