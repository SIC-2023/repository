#include "ProjectWindow.h"

#include "imgui.h"

namespace argent::editor
{
	ProjectWindow::ProjectWindow():
		EditorWindow("Project")
	{
		
	}

	void ProjectWindow::Awake()
	{

	}

	void ProjectWindow::OnShutdown()
	{
		
	}

	void ProjectWindow::OnRender()
	{
		ImGui::Begin(GetName().c_str());

		ImGui::End();
	}
}
