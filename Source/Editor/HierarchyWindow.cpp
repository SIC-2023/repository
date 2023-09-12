#include "HierarchyWindow.h"

#include "imgui.h"

#include "../Core/Engine.h"
#include "../Scene/SceneManager.h"

namespace argent::editor
{
	HierarchyWindow::HierarchyWindow():
		EditorWindow("Hierarchy")
	{
	}

	void HierarchyWindow::Awake()
	{
		EditorWindow::Awake();
	}

	void HierarchyWindow::OnShutdown()
	{
		EditorWindow::OnShutdown();
	}

	void HierarchyWindow::OnRender()
	{
		ImGui::Begin(GetName().c_str());

		GetEngine()->GetSubsystemLocator().Get<scene::SceneManager>()->GetCurrentScene()->OnGui();

		ImGui::End();
	}
}