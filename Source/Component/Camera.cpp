#include "Camera.h"

#include "../Core/Engine.h"
#include "../Scene/SceneManager.h"
#include "../Input/InputManager.h"

#include "imgui.h"


namespace argent::component
{
	Camera::Camera() :
		Component("Camera")
	{
		//GetEngine()->GetSubsystemLocator().Get<scene::SceneManager>()->GetCurrentScene()->RegisterCamera(this);
	}

	void Camera::Awake()
	{
		GetEngine()->GetSubsystemLocator().Get<scene::SceneManager>()->GetCurrentScene()->RegisterCamera(this);
	}

	void Camera::Update()
	{
		Controller();
	}

	void Camera::OnGui()
	{
		if(ImGui::TreeNode(GetName().c_str()))
		{
			ImGui::DragFloat("Move_speed", &move_speed_, 0.1f, 0.1f, 100.0f);
			ImGui::DragFloat("Rotation_Speed", &rotation_speed_, 0.001f, 0.001f, 1.57f);
			ImGui::DragFloat("FovAngle", &fov_angle_, 0.001f, 10.0f / 180 * 3.14f, 3.14f);
			ImGui::TreePop();
		}
	}

	void Camera::Controller()
	{
		auto& keyboard = argent::GetEngine()->GetSubsystemLocator().Get<argent::input::InputManager>()->GetKeyboard();
		auto& mouse = argent::GetEngine()->GetSubsystemLocator().Get<argent::input::InputManager>()->GetMouse();
		using namespace argent::input;
		DirectX::XMFLOAT3 position = GetOwner()->GetTransform()->GetPosition();
		DirectX::XMFLOAT3 rotation = GetOwner()->GetTransform()->GetRotation();
		if (mouse.GetButton(argent::input::MouseButton::RButton)/*�E�N���b�N�̓���*/)
		{
			//�J�����̉�]
			{
				const float dx = mouse.GetMovedVec().x;	//�}�E�XX�������̈ړ��l
				const float dy = mouse.GetMovedVec().y;	//�}�E�XY�������̈ړ��l

				const float x_d_angle = dy * rotation_speed_;
				const float y_d_angle = dx * rotation_speed_;

				rotation.x += x_d_angle;
				rotation.y += y_d_angle;
			}

			//�J�����̈ړ�
			{
				//�������Z�o
				const auto rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
				const DirectX::XMVECTOR front = DirectX::XMVector3Normalize(rotation_matrix.r[2]);
				const DirectX::XMVECTOR up = {0.0f, 1.0f, 0.0f, 1.0f};
				const DirectX::XMVECTOR right = DirectX::XMVector3Normalize(rotation_matrix.r[0]);

				float front_input = 0;
				float right_input = 0;
				float up_input = 0;

				//�O��
				if (keyboard.GetKey(W)/* Keyboard W */) front_input += 1.0f;
				if (keyboard.GetKey(S)/* Keyboard S */) front_input -= 1.0f;

				//���E
				if (keyboard.GetKey(A)/* Keyboard A */) right_input -= 1.0f;
				if (keyboard.GetKey(D)/* Keyboard D */) right_input += 1.0f;

				//�㉺
				if (keyboard.GetKey(E)/* Keyboard E */) up_input += 1.0f;
				if (keyboard.GetKey(Q)/* Keyboard Q */) up_input -= 1.0f;

				using namespace DirectX;

				const DirectX::XMVECTOR move_vector = DirectX::XMVector3Normalize(front * front_input + up * up_input + right * right_input);
				DirectX::XMStoreFloat3(&position, DirectX::XMLoadFloat3(&position) + move_vector * move_speed_);
			}

			GetOwner()->GetTransform()->SetPosition(position);
			GetOwner()->GetTransform()->SetRotation(rotation);
		}
	}

	DirectX::XMFLOAT3 Camera::GetPosition() const
	{
		return GetOwner()->GetTransform()->GetPosition();
	}

	DirectX::XMFLOAT4X4 Camera::GetViewProjection() const
	{
		using namespace DirectX;
		const DirectX::XMFLOAT3 position = GetOwner()->GetTransform()->GetPosition();
		const DirectX::XMFLOAT3 rotation = GetOwner()->GetTransform()->GetRotation();

		DirectX::XMFLOAT4X4 ret{};
		auto R = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		auto forward = R.r[2];
		forward = DirectX::XMVector3Normalize(forward);

		auto Eye = DirectX::XMLoadFloat3(&position);
		auto Focus = Eye + forward;
		Eye.m128_f32[3] = Focus.m128_f32[3] = 1.0f;
		auto V = DirectX::XMMatrixLookAtLH(Eye, Focus, up_);
		auto P = DirectX::XMMatrixPerspectiveFovLH(fov_angle_, aspect_ratio_, near_panel_, far_panel_);

		DirectX::XMStoreFloat4x4(&ret, V * P);
		return ret;
	}
	
}
