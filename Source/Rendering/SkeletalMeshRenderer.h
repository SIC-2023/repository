#pragma once

#include <memory>

#include "../Graphics/ConstantBuffer.h"
#include "../Graphics/GraphicsPipelineState.h"
#include "../Graphics/Model.h"

#include "RenderContext.h"

namespace argent::rendering
{
	/**
	 * \brief ボーンを持っているモデル描画のクラス
	 * アニメーションをする場合に使う
	 */
	class SkeletalMeshRenderer
	{
	public:
		struct ObjectConstant
		{
			DirectX::XMFLOAT4X4 world_;
		};

		struct SkeletalMeshConstant
		{
			DirectX::XMFLOAT4X4 global_transform_{};
			DirectX::XMFLOAT4X4 bone_transform_[256]{};
		};

		SkeletalMeshRenderer(ID3D12Device8* device, std::shared_ptr<graphics::Model> model);

		virtual ~SkeletalMeshRenderer() = default;

		void Render(const RenderContext& render_context, const DirectX::XMFLOAT4X4& world) const;
	private:
		std::unique_ptr<graphics::GraphicsPipelineState> graphics_pipeline_state_{};
		std::shared_ptr<graphics::Model> model_{};

		std::unique_ptr<graphics::ConstantBuffer<ObjectConstant>> object_constant_buffers_[3]{};
		std::vector<std::unique_ptr<graphics::ConstantBuffer<SkeletalMeshConstant>>> mesh_constant_buffers_[3]{};
	};
}

