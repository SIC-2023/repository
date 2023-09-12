#include "Camera.h"

#include "../Input/InputManager.h"
#include "../Core/Engine.h"

#include "imgui.h"


namespace argent::component
{
	Camera::Camera() :
		Component("Camera")
	{
		
	}

	void Camera::OnGui()
	{
		if(ImGui::TreeNode(GetName().c_str()))
		{
			ImGui::DragFloat("Move_speed", &move_speed_, 0.1f, 0.1f, 100.0f);
			ImGui::DragFloat("Rotation_Speed", &rotation_speed_, 0.001f, 0.001f, 1.57f);
			ImGui::DragFloat3("Position", &position_.x, 0.01f, -FLT_MAX, FLT_MAX);
			ImGui::DragFloat3("Rotate", &rotation_.x, 0.01f, -FLT_MAX, FLT_MAX);
			ImGui::DragFloat("FovAngle", &fov_angle_, 0.001f, 10.0f / 180 * 3.14f, 3.14f);
			ImGui::TreePop();
		}
	}

	void Camera::Controller()
	{
		auto& keyboard = argent::GetEngine()->GetSubsystemLocator().Get<argent::input::InputManager>()->GetKeyboard();
		auto& mouse = argent::GetEngine()->GetSubsystemLocator().Get<argent::input::InputManager>()->GetMouse();
		using namespace argent::input;
		if (mouse.GetButton(argent::input::MouseButton::RButton)/*右クリックの入力*/)
		{
			//カメラの回転
			{
				const float dx = mouse.GetMovedVec().x;	//マウスX軸方向の移動値
				const float dy = mouse.GetMovedVec().y;	//マウスY軸方向の移動値

				const float x_d_angle = dy * rotation_speed_;
				const float y_d_angle = dx * rotation_speed_;

				rotation_.x += x_d_angle;
				rotation_.y += y_d_angle;
			}

			//カメラの移動
			{
				//方向を算出
				const auto rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);
				const DirectX::XMVECTOR front = DirectX::XMVector3Normalize(rotation_matrix.r[2]);
				const DirectX::XMVECTOR up = {0.0f, 1.0f, 0.0f, 1.0f};
				const DirectX::XMVECTOR right = DirectX::XMVector3Normalize(rotation_matrix.r[0]);

				float front_input = 0;
				float right_input = 0;
				float up_input = 0;

				//前後
				if (keyboard.GetKey(W)/* Keyboard W */) front_input += 1.0f;
				if (keyboard.GetKey(S)/* Keyboard S */) front_input -= 1.0f;

				//左右
				if (keyboard.GetKey(A)/* Keyboard A */) right_input -= 1.0f;
				if (keyboard.GetKey(D)/* Keyboard D */) right_input += 1.0f;

				//上下
				if (keyboard.GetKey(E)/* Keyboard E */) up_input += 1.0f;
				if (keyboard.GetKey(Q)/* Keyboard Q */) up_input -= 1.0f;

				using namespace DirectX;

				const DirectX::XMVECTOR move_vector = DirectX::XMVector3Normalize(front * front_input + up * up_input + right * right_input);
				DirectX::XMFLOAT3 p = { position_.x, position_.y, position_.z };
				DirectX::XMStoreFloat3(&p, DirectX::XMLoadFloat3(&p) + move_vector * move_speed_);
				position_ = { p.x, p.y, p.z, 1.0f };
			}

		}
	}

	DirectX::XMFLOAT4X4 Camera::GetViewProjection() const
	{
		using namespace DirectX;

		DirectX::XMFLOAT4X4 ret{};
		auto R = DirectX::XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);
		auto forward = R.r[2];
		forward = DirectX::XMVector3Normalize(forward);

		auto Eye = DirectX::XMLoadFloat4(&position_);
		auto Focus = Eye + forward;
		Eye.m128_f32[3] = Focus.m128_f32[3] = 1.0f;
		auto V = DirectX::XMMatrixLookAtLH(Eye, Focus, up_);
		auto P = DirectX::XMMatrixPerspectiveFovLH(fov_angle_, aspect_ratio_, near_panel_, far_panel_);

		DirectX::XMStoreFloat4x4(&ret, V * P);
		return ret;
	}
	
}
