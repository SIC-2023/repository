#pragma once

#include <DirectXMath.h>


#include "../SubSystem/Subsystem.h"

namespace argent::editor
{
	class Editor final : public Subsystem
	{
	private:
		struct Camera
		{
			DirectX::XMFLOAT3 position_;
			DirectX::XMFLOAT3 rotation_;
			float near_z_;
			float far_z_;
			float aspect_ratio_;
			float fov_angle_;
		};

	public:
		Editor() = default;
		~Editor() override = default;

		void OnAwake() override;
		void OnShutdown() override;

		void Update();
		void Render();
	private:
		Camera editor_camera_;
	};
}

