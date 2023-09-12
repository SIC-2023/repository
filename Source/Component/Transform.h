#pragma once

#include <DirectXMath.h>
#include "Component.h"

namespace argent::component
{
	class Transform final :
		public Component
	{
	public:
		Transform();
		~Transform() override = default;

		DirectX::XMFLOAT4X4 CalcWorldMatrix() const;

		void OnGui() override;

		DirectX::XMFLOAT3 GetPosition() const { return position_; }
		DirectX::XMFLOAT3 GetScale() const { return scale_; }
		DirectX::XMFLOAT3 GetRotation() const { return rotation_; }

		void SetPosition(const DirectX::XMFLOAT3& f) { position_ = f; }
		void SetScale(const DirectX::XMFLOAT3& f) { scale_ = f; }
		void SetRotation(const DirectX::XMFLOAT3& f) { rotation_ = f; }
	private:
		DirectX::XMFLOAT3 position_;
		DirectX::XMFLOAT3 scale_{ 1, 1, 1 };
		DirectX::XMFLOAT3 rotation_;

		DirectX::XMFLOAT4X4 coordinate_system
		{
			-1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1,
		};
	};
}

