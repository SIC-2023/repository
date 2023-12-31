#include "StaticMeshRenderer.h"

#include "imgui.h"

#include "d3dx12.h"
#include "../Graphics/GraphicsContext.h"
#include "RenderContext.h"

struct RenderResource
{
	uint32_t scene_constant_index_;
	uint32_t object_constant_index_;
	uint32_t mesh_constant_index_;
	uint32_t material_constant_index_;
	uint32_t vertices_index_;
	uint32_t vertex_indices_index_;
	uint32_t meshlets_index_;
	uint32_t primitives_index_;
	uint32_t frustum_;
	uint32_t instance_index_;
	uint32_t position_index_;
	uint32_t normal_index_;
	uint32_t tangent_index_;
	uint32_t binormal_index_;
	uint32_t texcoord_index_;
};

namespace argent::rendering
{
	StaticMeshRenderer::StaticMeshRenderer(const graphics::GraphicsContext& graphics_context, 
		const std::shared_ptr<graphics::Model>& model, const wchar_t* ps_filename)
	{
		auto* device = graphics_context.device_;
		auto* cbv_descriptor = graphics_context.cbv_srv_uav_heap_;
		for (int i = 0; i < model->GetNumMeshes(); ++i)
		{
			auto& mesh_constant_buffer0 = mesh_constant_buffers_[0].emplace_back();
			auto& mesh_constant_buffer1 = mesh_constant_buffers_[1].emplace_back();
			auto& mesh_constant_buffer2 = mesh_constant_buffers_[2].emplace_back();
			mesh_constant_buffer0 = std::make_unique<graphics::ConstantBuffer<MeshConstant>>(device, cbv_descriptor->PopDescriptor());
			mesh_constant_buffer1 = std::make_unique<graphics::ConstantBuffer<MeshConstant>>(device, cbv_descriptor->PopDescriptor());
			mesh_constant_buffer2 = std::make_unique<graphics::ConstantBuffer<MeshConstant>>(device, cbv_descriptor->PopDescriptor());
		}

		object_constant_buffer_[0] = std::make_unique<graphics::ConstantBuffer<ObjectConstant>>(device, cbv_descriptor->PopDescriptor());
		object_constant_buffer_[1] = std::make_unique<graphics::ConstantBuffer<ObjectConstant>>(device, cbv_descriptor->PopDescriptor());
		object_constant_buffer_[2] = std::make_unique<graphics::ConstantBuffer<ObjectConstant>>(device, cbv_descriptor->PopDescriptor());
		model_ = model;

		graphics::GraphicsPipelineDesc graphics_pipeline_desc{};
		graphics_pipeline_desc.vs_filename_ = L"./Assets/Shader/StaticMeshVS.hlsl";
		graphics_pipeline_desc.ps_filename_ = ps_filename;
		graphics_pipeline_desc.independent_input_layout_buffer_enable_ = true;
		graphics_pipeline_desc.rasterizer_mode_ = graphics::dx12::RasterizerMode::CullBackSolid;
		graphics_pipeline_state_ = std::make_unique<graphics::GraphicsPipelineState>(device, graphics_pipeline_desc, L"StaticMeshPipeline");

		//Mesh Shader
		graphics::MeshShaderPipelineDesc mesh_shader_pipeline_desc{};
		mesh_shader_pipeline_desc.ms_filename_ = L"./Assets/Shader/DemoMeshShaderAMS.hlsl";
		mesh_shader_pipeline_desc.as_filename_ = L"./Assets/Shader/DemoMeshShaderAS.hlsl";
		mesh_shader_pipeline_desc.ps_filename_ = L"./Assets/Shader/DemoMeshShaderPS.hlsl";
		mesh_shader_pipeline_desc.rasterizer_mode_ = graphics::dx12::RasterizerMode::CullBackSolid;
		mesh_shader_pipeline_state_ = std::make_unique<graphics::GraphicsPipelineState>(device, mesh_shader_pipeline_desc);
	}

	void StaticMeshRenderer::Render(const RenderContext& render_context, const DirectX::XMFLOAT4X4& world)
	{
		auto graphics_command_list = render_context.GetCommandList();


		RenderResource render_resource{};

		//シーンコンスタントとフラスタム
		render_resource.scene_constant_index_ = render_context.GetSceneConstantHeapIndex();
		render_resource.frustum_ = render_context.GetFrustumConstantHeapIndex();

		//オブジェクトコンスタント
		ObjectConstant object_constant{ world };
		object_constant_buffer_[render_context.GetFrameIndex()]->UpdateConstantBuffer(object_constant);
		render_resource.object_constant_index_ = static_cast<uint32_t>(object_constant_buffer_[render_context.GetFrameIndex()]->GetHeapIndex());

		for(size_t mesh_index = 0; mesh_index < model_->GetNumMeshes(); ++mesh_index)
		{
			const auto& mesh = model_->GetMesh(mesh_index);

			//メッシュコンスタント
			MeshConstant mesh_constant{};
			mesh_constant.local_transform_ = mesh->GetData().transform_;
			const auto& mesh_constant_buffer = mesh_constant_buffers_[render_context.GetFrameIndex()].at(mesh_index);
			mesh_constant_buffer->UpdateConstantBuffer(mesh_constant);
			render_resource.mesh_constant_index_ = static_cast<uint32_t>(mesh_constant_buffer->GetHeapIndex());

			//マテリアル
			const auto& material = model_->GetMaterial(mesh->GetMaterialIndex());
			material->UpdateConstantBuffer();
			render_resource.material_constant_index_ = static_cast<uint32_t>(material->GetHeapIndex());

			if(!use_mesh_shader_)
			{
				//頂点シェーダーバージョン
				graphics_pipeline_state_->SetOnCommandList(graphics_command_list);
				graphics_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				mesh->IASetBuffers(graphics_command_list);
				graphics_command_list->SetGraphicsRoot32BitConstants(0u, sizeof(render_resource) / 4, &render_resource, 0u);
				graphics_command_list->DrawIndexedInstanced(static_cast<UINT>(mesh->GetIndexCount()), 1, 0u, 0u, 0u);
			}
			else
			{
				//メッシュシェーダーバージョン
				mesh_shader_pipeline_state_->SetOnCommandList(graphics_command_list);

				//頂点データ
				render_resource.position_index_ = mesh->GetPositionBufferHeapIndex();
				render_resource.normal_index_ = mesh->GetNormalBufferHeapIndex();
				render_resource.tangent_index_ = mesh->GetTangentBufferHeapIndex();
				render_resource.binormal_index_ = mesh->GetBinormalBufferHeapIndex();
				render_resource.texcoord_index_ = mesh->GetTexcoordBufferHeapIndex();

				//アクセス用のバッファ
				render_resource.vertex_indices_index_ = mesh->GetVertexIndexHeapIndex();
				render_resource.meshlets_index_ = mesh->GetMeshletHeapIndex();
				render_resource.primitives_index_ = mesh->GetPrimitiveHeapIndex();

				graphics_command_list->SetGraphicsRoot32BitConstants(0u, sizeof(render_resource) / 4, &render_resource, 0u);
				graphics_command_list->DispatchMesh((static_cast<UINT>(mesh->GetNumMeshlets()) + 32u - 1u) / 32, 1, 1);
			}
		}
	}

	void StaticMeshRenderer::DrawOnGui()
	{
		if (ImGui::TreeNode("StaticMesh"))
		{
			model_->OnGui();
			ImGui::Checkbox("Using Mesh Shader", &use_mesh_shader_);
			ImGui::Text(use_mesh_shader_ ? "Mesh Shader" : "Vertex Shader");

			int mesh = static_cast<int>(model_->GetNumMeshes());
			ImGui::InputInt("Mesh Counts", &mesh);
			int polygon = model_->GetNumPolygons();
			ImGui::InputInt("Polygon Counts", &polygon);

			ImGui::TreePop();
		}
	}
}
