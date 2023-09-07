#include "RenderContext.h"

#include "../Graphics/GraphicsEngine.h"

namespace argent::graphics
{
	//TODO graphicsEngine‚©‚ç‚¾‚¢‚½‚¢Žæ‚Á‚Ä‚±‚ê‚éî•ñ‚¾‚©‚çA
	//ˆø”‚ÍGraphicsEngine*‚¾‚¯‚Å‚à‚¢‚¢‹C‚ª‚·‚é
	RenderContext::RenderContext(const GraphicsEngine* graphics_engine, int frame_resource_index,
		uint32_t scene_constant_buffer_heap_index, uint32_t frustum_constant_buffer_heap_index,
		GraphicsCommandList* graphics_command_list, D3D12_VIEWPORT viewport):
		graphics_engine_(graphics_engine)
	,	frame_index_(frame_resource_index)
	,	scene_constant_buffer_heap_index_(scene_constant_buffer_heap_index)
	,	frustum_constant_buffer_heap_index_(frustum_constant_buffer_heap_index)
	,	graphics_command_list_(graphics_command_list)
	,	viewport_(viewport)
	{}

	void RenderContext::Begin() const
	{
		graphics_command_list_->Begin();
	}

	void RenderContext::End(ID3D12CommandQueue* command_queue) const
	{
		graphics_command_list_->Close();
		ID3D12CommandList* execute_command_lists[]{ graphics_command_list_->GetCommandList() };
		command_queue->ExecuteCommandLists(_countof(execute_command_lists), execute_command_lists);
	}

	void RenderContext::SetSceneData(const SceneConstant& scene_constant) const
	{
		graphics_engine_->UpdateSceneConstantBuffer(scene_constant);
	}

	void RenderContext::SetFrustumData(const DirectX::XMFLOAT4X4& view_projection,
	                                   const DirectX::XMFLOAT4& camera_position) const
	{
		graphics_engine_->UpdateFrustumConstantBuffer(view_projection, camera_position);
	}
}
