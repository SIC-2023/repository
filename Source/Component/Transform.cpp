#include "Transform.h"

#include"imgui.h"

namespace argent::component
{
	Transform::Transform():
		Component("Transform")
	{

	}

	void Transform::OnDrawInspector()
	{
		if(ImGui::TreeNode("Transform"))
		{
			ImGui::DragFloat3("Position", &position_.x, 0.1f, -500.0f, 500.0f);
			ImGui::DragFloat3("Scale", &scale_.x, 0.01f, -100.0f, 100.0f);
			ImGui::DragFloat3("Rotation", &rotation_.x, 0.001f, -3.14f, 3.14f);
			ImGui::TreePop();
		}
	}

	void Transform::OnGui()
	{
		if(ImGui::TreeNode("Transform"))
		{
			ImGui::DragFloat3("Position", &position_.x, 0.1f, -500.0f, 500.0f);
			ImGui::DragFloat3("Scale", &scale_.x, 0.01f, -100.0f, 100.0f);
			ImGui::DragFloat3("Rotation", &rotation_.x, 0.001f, -3.14f, 3.14f);
			ImGui::TreePop();
		}
	}
}
