#include "Transform.h"

#include"imgui.h"

namespace argent::component
{
	Transform::Transform():
		Component("Transform")
	{

	}

	DirectX::XMFLOAT4X4 Transform::CalcWorldMatrix() const
	{
		DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&coordinate_system);
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale_.x, scale_.y, scale_.z);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position_.x, position_.y, position_.z);

		DirectX::XMMATRIX M = C * S * R * T;
		DirectX::XMFLOAT4X4 ret{};
		DirectX::XMStoreFloat4x4(&ret, M);
		return ret;
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
