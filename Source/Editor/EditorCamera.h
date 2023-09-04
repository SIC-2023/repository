#pragma once

#include <DirectXMath.h>

namespace argent::editor
{
	class EditorCamera
	{
	public:
		EditorCamera() = default;
		virtual ~EditorCamera() = default;

		void Update();

	private:

		DirectX::XMFLOAT3 position_{};
		DirectX::XMFLOAT3 rotation_{};

		float aspect_ratio_{};
		float fov_angle_{};
		float near_z_{};
		float far_z_{};

		float move_speed_{};
		float rotation_speed_{};
	};
}

