#pragma once

#include <DirectXMath.h>

#include "Component.h"

namespace argent::component
{
	class Light :
		public Component 
	{
	public:
		Light();
		~Light() override = default;


		void Awake() override;
		void OnGui() override;

		DirectX::XMFLOAT3 GetColor() const { return color_; }
		float GetIntensity() const { return intensity_; }
		DirectX::XMFLOAT3 GetDirection() const { return direction_; }
	private:
		DirectX::XMFLOAT3 direction_{ 1.0f, -1.0f, 1.0f };
		DirectX::XMFLOAT3 color_{ 1.0f, 1.0f, 1.0f };
		float intensity_;
	};
	
}

