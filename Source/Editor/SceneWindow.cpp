#include "SceneWindow.h"

#include "imgui.h"

#include "Editor.h"

namespace argent::editor
{
	SceneWindow::SceneWindow():
		EditorWindow("Scene")
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
		ImGui::Begin(GetName().c_str(), nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
		const ImVec2 window_size =  ImGui::GetWindowSize();

		ImGui::Image(reinterpret_cast<ImTextureID>(Editor::GetSceneSrvHeapIndex()), window_size);

		ImGui::End();
	}

}
