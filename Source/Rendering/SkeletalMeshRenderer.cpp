#include "SkeletalMeshRenderer.h"

#include "../../Assets/Shader/RenderResource.h"

namespace argent::rendering
{
	SkeletalMeshRenderer::SkeletalMeshRenderer(ID3D12Device8* device, std::shared_ptr<graphics::Model> model)
	{
		model_ = model;

		graphics::GraphicsPipelineDesc gp_desc{};
		gp_desc.vs_filename_ = L"./Assets/Shader/SkeletalMeshVS.hlsl";
		gp_desc.ps_filename_ = L"./Assets/Shader/SkeletalMeshPS.hlsl";
		gp_desc.rasterizer_mode_ = graphics::dx12::RasterizerMode::CullBackSolid;
		graphics_pipeline_state_ = std::make_unique<graphics::GraphicsPipelineState>(device, gp_desc, L"SkeletalMeshPipeline");
	}

	void SkeletalMeshRenderer::Render(const RenderContext& render_context, const DirectX::XMFLOAT4X4& world) const
	{
		const auto graphics_command_list = render_context.GetCommandList();

		const auto object_constant_buffer = object_constant_buffers_[render_context.GetFrameIndex()].get();
		ObjectConstant object_constant{};
		object_constant.world_ = world;
		object_constant_buffer->UpdateConstantBuffer(object_constant);

		graphics::AnimationClip::Keyframe keyframe{};
		for(uint32_t mesh_index = 0; mesh_index < model_->GetNumMeshes(); ++mesh_index)
		{
			const auto& mesh = model_->GetMesh(mesh_index);
			const auto& skeletal_mesh_constant_buffer = mesh_constant_buffers_[render_context.GetFrameIndex()].at(mesh_index);
			//Update Constant Buffer
			SkeletalMeshConstant skeletal_mesh_constant{};
			skeletal_mesh_constant.global_transform_ = mesh->GetData().transform_;

			SkeletalMeshRenderResource render_resource;
			render_resource.scene_constant_ = render_context.GetSceneConstantHeapIndex();
			render_resource.object_constant_ = static_cast<uint32_t>(object_constant_buffer->GetHeapIndex());

			graphics_command_list->SetGraphicsRoot32BitConstants(0u, sizeof(SkeletalMeshRenderResource) / 4, &render_resource, 0u);

			graphics_command_list->DrawIndexedInstanced(static_cast<UINT>(mesh->GetIndexCount()), 1u, 0u, 0u, 0u);
		}
	}
}
