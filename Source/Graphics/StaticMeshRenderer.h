#pragma once


#include "ConstantBuffer.h"
#include "GraphicsPipelineState.h"
#include "Model.h"

namespace argent::graphics
{
	struct GraphicsContext;
	class RenderContext;

	/**
	 * \brief ボーンなし3Dモデル描画クラス
	 */
	class StaticMeshRenderer
	{
	public:
		struct ObjectConstant
		{
			DirectX::XMFLOAT4X4 world_;
		};

		struct MeshConstant
		{
			DirectX::XMFLOAT4X4 local_transform_;
		};

	public:
		StaticMeshRenderer(const GraphicsContext& graphics_context, const std::shared_ptr<Model>& model);
		//StaticMeshRenderer(ID3D12Device8* device, dx12::DescriptorHeap* cbv_descriptor, const std::shared_ptr<Model> model);

		void Render(const RenderContext& render_context, const DirectX::XMFLOAT4X4& world);

	private:

		//スタティックメッシュメンバ
		std::unique_ptr<GraphicsPipelineState> graphics_pipeline_state_{};
		std::unique_ptr<GraphicsPipelineState> mesh_shader_pipeline_state_{};
		std::shared_ptr<Model> model_{};


		std::unique_ptr<ConstantBuffer<ObjectConstant>> object_constant_buffer_[3]{};
		std::vector<std::unique_ptr<ConstantBuffer<MeshConstant>>> mesh_constant_buffers_[3]{};

		//TODO 消しなさい
		bool use_mesh_shader_ = false;
	};
}

