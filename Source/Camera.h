#pragma once
#include <DirectXMath.h>


class Camera
{
public:
	Camera() = default;

	//TODO ImGuiへの描画を行う関数名の変更
	void OnGui();

	//TODO カメラコントローラー　Editor専用なので移動させること
	void Controller();

	DirectX::XMFLOAT4 GetPosition() const { return position_; }
	DirectX::XMFLOAT4X4 GetViewProjection() const;
private:


	DirectX::XMFLOAT4 position_{ 0, 0, 0, 1.0f };
	DirectX::XMFLOAT3 rotation_;
	DirectX::XMVECTOR up_{ 0, 1, 0, 0 };
	float fov_angle_ = DirectX::XMConvertToRadians(60.0f);
	float aspect_ratio_ = 1280.0f / 720.0f;
	float near_panel_ = 0.01f;
	float far_panel_ = 1000.0f;

	//TODO Editor専用のやつなので移動させる
	float move_speed_;
	float rotation_speed_;
};

