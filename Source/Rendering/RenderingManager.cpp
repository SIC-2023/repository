#include "RenderingManager.h"

#include "RenderContext.h"
#include "../Scene/BaseScene.h"

#include "../Core/Engine.h"
#include "../Graphics/GraphicsContext.h"
#include "../Graphics/GraphicsEngine.h"

#include "../Editor/Editor.h"
#include "../Graphics/Texture.h"

namespace argent::rendering
{
	RenderingManager::RenderingManager()
	{

	}

	RenderingManager::~RenderingManager()
	{

	}

	void RenderingManager::Awake()
	{
		const DXGI_FORMAT render_target_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		const auto graphics_context = GetEngine()->GetSubsystemLocator().Get<graphics::GraphicsEngine>()->GetGraphicsContext();

		for(int i = 0; i < graphics::kNumBackBuffers; ++i)
		{
			frame_resource_[i] = std::make_unique<graphics::FrameResource>(graphics_context.device_, graphics_context.swap_chain_, i,
				graphics_context.rtv_heap_->PopDescriptor(), graphics_context.dsv_heap_->PopDescriptor(), 
				graphics_context.cbv_srv_uav_heap_->PopDescriptor(), graphics_context.cbv_srv_uav_heap_->PopDescriptor());

			frame_buffers_[i] = std::make_unique<graphics::FrameBuffer>(graphics_context, 1280, 720, render_target_format);
		}

		post_process_manager_.Awake(graphics_context);

		graphics::GraphicsPipelineDesc graphics_pipeline_desc{};
		graphics_pipeline_desc.vs_filename_ = L"./Assets/Shader/FullscreenQuadVS.hlsl";
		graphics_pipeline_desc.ps_filename_ = L"./Assets/Shader/FullscreenQuadPS.hlsl";
		graphics_pipeline_desc.rtv_format_[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		graphics_pipeline_state_ = std::make_unique<graphics::GraphicsPipelineState>(graphics_context.device_, graphics_pipeline_desc, L"FullscreenQuad");

		sky_map_texture_ = std::make_unique<graphics::Texture>(graphics_context, L"./Assets/Skymap.dds");
		graphics_pipeline_desc.depth_mode_ = graphics::dx12::DepthMode::TestOffWriteOff;
		graphics_pipeline_desc.rasterizer_mode_ = graphics::dx12::RasterizerMode::CullNoneSolid;
		graphics_pipeline_desc.ps_filename_ = L"./Assets/Shader/SkymapPS.hlsl";
		graphics_pipeline_desc.rtv_format_[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		sky_map_graphics_pipeline_state_ = std::make_unique<graphics::GraphicsPipelineState>(graphics_context.device_, graphics_pipeline_desc, L"SkyMap");
		Subsystem::Awake();
	}

	void RenderingManager::Shutdown()
	{
		post_process_manager_.Shutdown();
	}
	
	void RenderingManager::Execute(const RenderContext& render_context, scene::BaseScene* scene)
	{
		//描画開始
		auto* d3d12_command_list = render_context.GetCommandList();
		const int frame_index = render_context.GetFrameIndex();
		const D3D12_VIEWPORT viewport = render_context.GetViewport();
		const D3D12_RECT rect { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };

		d3d12_command_list->RSSetViewports(1u, &viewport);
		d3d12_command_list->RSSetScissorRects(1u, &rect);

		frame_buffers_[frame_index]->Begin(d3d12_command_list);

		////シーン描画
		scene->Render(render_context);

		d3d12_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		sky_map_graphics_pipeline_state_->SetOnCommandList(d3d12_command_list);
		struct RenderResource
		{
			uint32_t texture_index_;
			uint32_t scene_constant_index_;
		} render_resource { sky_map_texture_->GetHeapIndex(), render_context.GetSceneConstantHeapIndex() };

		d3d12_command_list->SetGraphicsRoot32BitConstants(0u, 2u, &render_resource, 0u);
		d3d12_command_list->DrawInstanced(4u, 1u, 0u, 0u);

		for(auto& renderer : renderer_components_)
		{
			if(renderer->GetIsActive() && renderer->GetOwner()->GetIsActive())
			{
				renderer->OnRender(render_context);
			}
		}

		frame_buffers_[frame_index]->End(d3d12_command_list);

		//ポストプロセス
		post_process_manager_.Execute(render_context, static_cast<uint32_t>(frame_buffers_[frame_index]->GetSrvHeapIndex()));

		//最終出力
		constexpr float clear_color[4] { 1, 1, 1, 1 };
		frame_resource_[frame_index]->Begin(d3d12_command_list, viewport, rect, clear_color);

		const uint64_t scene_srv_heap_index = is_post_processing_ ? 
			post_process_manager_.GetSrvHeapIndex() : frame_buffers_[frame_index]->GetSrvHeapIndex();

		//エディタモードならGuiを描画 and シーンをシーンウィンドウ上に出力
		//offならそのまま出力
		if(GetEngine()->GetIsEditorMode())
		{
			GetEngine()->GetSubsystemLocator().Get<editor::Editor>()->OnRender(render_context, scene_srv_heap_index);
		}
		else
		{
			graphics_pipeline_state_->SetOnCommandList(d3d12_command_list);
			d3d12_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			struct FullscreenQuadRenderResource
			{
				uint32_t texture_index_;
			} const render_resource{ static_cast<uint32_t>(scene_srv_heap_index) };

			d3d12_command_list->SetGraphicsRoot32BitConstants(0u, 1u, &render_resource, 0u);
			d3d12_command_list->DrawInstanced(4u, 1u, 0u, 0u);
		}

		//描画終了
		frame_resource_[frame_index]->End(d3d12_command_list);
	}
}
