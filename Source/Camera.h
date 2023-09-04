#pragma once
#include <DirectXMath.h>


class Camera
{
public:
	Camera() = default;

	//TODO ÉäÉlÅ[ÉÄ
	void OnGui();

	//TODO è¡Ç∑
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

	//TODO kese
	float move_speed_;
	float rotation_speed_;
};

