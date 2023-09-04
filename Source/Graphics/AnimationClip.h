#pragma once

#include <DirectXMath.h>

#include <string>
#include <vector>

namespace argent::graphics
{
	struct AnimationClip
	{
	public:

		struct Keyframe
		{
			struct Node
			{
				std::string name_;
				DirectX::XMFLOAT3 translation_;
				DirectX::XMFLOAT3 scaling_;
				DirectX::XMFLOAT4 rotation_;
			};

			std::vector<Node> nodes_;
		};


	public:
		std::vector<Keyframe> sequence_;

	};
}