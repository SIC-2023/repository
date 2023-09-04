#include "EditorCamera.h"

#include "../Input/InputManager.h"


namespace argent::editor
{
	void EditorCamera::Update()
	{
		if (true/*右クリックの入力*/)
		{
			//カメラの移動
			{
				//方向を算出
				const auto rotation_matrix = DirectX::XMMatrixRotationRollPitchYaw(rotation_.x, rotation_.y, rotation_.z);
				const DirectX::XMVECTOR front = DirectX::XMVector3Normalize(rotation_matrix.r[2]);
				const DirectX::XMVECTOR up = DirectX::XMVector3Normalize(rotation_matrix.r[1]);
				const DirectX::XMVECTOR right = DirectX::XMVector3Normalize(rotation_matrix.r[0]);

				float front_input = 0;
				float right_input = 0;
				float up_input = 0;

				//前後
				if (true/* Keyboard W */) front_input += 1.0f;
				if (true/* Keyboard S */) front_input -= 1.0f;

				//左右
				if (true/* Keyboard A */) right_input -= 1.0f;
				if (true/* Keyboard D */) right_input += 1.0f;

				//上下
				if (true/* Keyboard Q */) up_input += 1.0f;
				if (true/* Keyboard E */) up_input -= 1.0f;

				using namespace DirectX;

				const DirectX::XMVECTOR move_vector = DirectX::XMVector3Normalize(front * front_input + up * up_input + right * right_input);
				DirectX::XMStoreFloat3(&position_, DirectX::XMLoadFloat3(&position_) + move_vector * move_speed_);
			}

			//カメラの回転
			{
				const float dx = 0;	//マウスX軸方向の移動値
				const float dy = 0;	//マウスY軸方向の移動値

				const float x_d_angle = dy * rotation_speed_;
				const float y_d_angle = dx * rotation_speed_;

				rotation_.x += x_d_angle;
				rotation_.y += y_d_angle;
			}
		}
	}
}