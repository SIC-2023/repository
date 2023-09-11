#include "RenderingManager.h"

#include "RenderContext.h"
#include "../Scene/BaseScene.h"

#include "../Core/Engine.h"
#include "../Graphics/GraphicsContext.h"
#include "../Graphics/GraphicsEngine.h"

#include "../Editor/Editor.h"

namespace argent::rendering
{
	RenderingManager::RenderingManager()
	{

	}

	RenderingManager::~RenderingManager()
	{

	}

	void RenderingManager::OnAwake()
	{
		const DXGI_FORMAT render_target_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		const auto graphics_context = GetEngine()->GetSubsystemLocator().Get<graphics::GraphicsEngine>()->GetGraphicsContext();

		for(int i = 0; i < graphics::kNumBackBuffers; ++i)
		{
			frame_resource_[i] = std::make_unique<graphics::FrameResource>(graphics_context.device_, graphics_context.swap_chain_, i,
				graphics_context.rtv_heap_->PopDescriptor(), graphics_context.dsv_heap_->PopDescriptor(), 
				graphics_context.cbv_srv_uav_heap_->PopDescriptor(), graphics_context.cbv_srv_uav_heap_->PopDescriptor());

			frame_buffers_[i] = std::make_unique<graphics::FrameBuffer>(graphics_context, 1280, 720, render_target_format);
		}

		post_process_manager_.OnAwake(graphics_context);

		graphics::GraphicsPipelineDesc graphics_pipeline_desc{};
		graphics_pipeline_desc.vs_filename_ = L"./Assets/Shader/FullscreenQuadVS.hlsl";
		graphics_pipeline_desc.ps_filename_ = L"./Assets/Shader/FullscreenQuadPS.hlsl";
		graphics_pipeline_state_ = std::make_unique<graphics::GraphicsPipelineState>(graphics_context.device_, graphics_pipeline_desc, L"FullscreenQuad");

		Subsystem::OnAwake();
	}

	void RenderingManager::OnShutdown()
	{
		post_process_manager_.OnShutdown();
	}
	
	void RenderingManager::Execute(const RenderContext& render_context, scene::BaseScene* scene)
	{
		//�`��J�n
		auto* d3d12_command_list = render_context.GetCommandList();
		const auto viewport = render_context.GetViewport();
		D3D12_RECT rect { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };

		d3d12_command_list->RSSetViewports(1u, &viewport);
		d3d12_command_list->RSSetScissorRects(1u, &rect);

		frame_buffers_[render_context.GetFrameIndex()]->Begin(d3d12_command_list);

		////�V�[���`��
		scene->Render(render_context);

		frame_buffers_[render_context.GetFrameIndex()]->End(d3d12_command_list);

		//�|�X�g�v���Z�X
		//post_process_manager_.Execute(render_context);

		//�ŏI�o��
		constexpr float clear_color[4] { 1, 1, 1, 1 };
		frame_resource_[render_context.GetFrameIndex()]->Begin(d3d12_command_list, viewport, rect, clear_color);

		//�G�f�B�^���[�h�Ȃ�Gui��`�� and �V�[�����V�[���E�B���h�E��ɏo��
		//off�Ȃ炻�̂܂܏o��
		if(GetEngine()->GetIsEditorMode())
		{
			GetEngine()->GetSubsystemLocator().Get<editor::Editor>()->OnRender(render_context, frame_buffers_[render_context.GetFrameIndex()]->GetSrvHeapIndex());
		}
		else
		{
			graphics_pipeline_state_->SetOnCommandList(d3d12_command_list);
			d3d12_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			struct FullscreenQuadRenderResource
			{
				uint32_t texture_index_;
			} render_resource{ frame_buffers_[render_context.GetFrameIndex()]->GetSrvHeapIndex() };
			d3d12_command_list->SetGraphicsRoot32BitConstants(0u, 1u, &render_resource, 0u);
			d3d12_command_list->DrawInstanced(4u, 1u, 0u, 0u);
			//scene->Render(render_context);
		}

		//�`��I��
		frame_resource_[render_context.GetFrameIndex()]->End(d3d12_command_list);
	}
}
