#include "RenderingManager.h"

#include "RenderContext.h"
#include "../Scene/BaseScene.h"

#include "../Core/Engine.h"
#include "../Graphics/GraphicsContext.h"
#include "../Graphics/GraphicsEngine.h"

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
		const auto graphics_context = GetEngine()->GetSubsystemLocator().Get<graphics::GraphicsEngine>()->GetGraphicsContext();

		for(int i = 0; i < graphics::kNumBackBuffers; ++i)
		{
			frame_resource_[i] = std::make_unique<graphics::FrameResource>(graphics_context.device_, graphics_context.swap_chain_, i,
				graphics_context.rtv_heap_->PopDescriptor(), graphics_context.dsv_heap_->PopDescriptor(), 
				graphics_context.cbv_srv_uav_heap_->PopDescriptor(), graphics_context.cbv_srv_uav_heap_->PopDescriptor());

			frame_buffers_[i] = std::make_unique<graphics::FrameBuffer>(graphics_context, 1280, 720, DXGI_FORMAT_R32G32B32A32_FLOAT);
		}

		post_process_manager_.OnAwake(graphics_context);

		Subsystem::OnAwake();
	}

	void RenderingManager::OnShutdown()
	{
		post_process_manager_.OnShutdown();
	}
	
	void RenderingManager::Execute(const RenderContext& render_context, scene::BaseScene* scene)
	{
		//描画開始
		const auto viewport = render_context.GetViewport();
		D3D12_RECT rect { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };

		//render_context.GetCommandList()->RSSetViewports(1u, &viewport);
		//render_context.GetCommandList()->RSSetScissorRects(1u, &rect);

		//frame_buffers_[render_context.GetFrameIndex()]->Begin(render_context.GetCommandList());

		////シーン描画
		//scene->Render(render_context);

		//frame_buffers_[render_context.GetFrameIndex()]->End(render_context.GetCommandList());

		////ポストプロセス
		//post_process_manager_.Execute(render_context);

		//最終出力
		constexpr float clear_color[4] { 1, 1, 1, 1 };
		frame_resource_[render_context.GetFrameIndex()]->Begin(render_context.GetCommandList(), viewport, rect, clear_color);

		scene->Render(render_context);

		//描画終了
		frame_resource_[render_context.GetFrameIndex()]->End(render_context.GetCommandList());
	}
}
