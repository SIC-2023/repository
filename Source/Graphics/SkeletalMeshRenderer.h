#pragma once

#include <memory>

#include "ConstantBuffer.h"
#include "GraphicsPipelineState.h"
#include "RenderContext.h"

#include "Model.h"

namespace argent::graphics
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

		SkeletalMeshRenderer(ID3D12Device8* device, std::shared_ptr<Model> model);

		virtual ~SkeletalMeshRenderer() = default;

		void Render(const RenderContext& render_context, const DirectX::XMFLOAT4X4& world) const;
	private:
		std::unique_ptr<GraphicsPipelineState> graphics_pipeline_state_{};
		std::shared_ptr<Model> model_{};


		std::unique_ptr<ConstantBuffer<ObjectConstant>> object_constant_buffers_[3]{};
		std::vector<std::unique_ptr<ConstantBuffer<SkeletalMeshConstant>>> mesh_constant_buffers_[3]{};
	};
}

