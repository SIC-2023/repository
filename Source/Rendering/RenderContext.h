#pragma once
#include <cstdint>
#include <d3d12.h>

#include "../Graphics/GraphicsCommandList.h"

#include "../../Assets/Shader/Constant.h"

namespace argent::graphics
{
	class GraphicsEngine;
}

namespace argent::rendering
{
	struct RenderTarget
	{
		const D3D12_CPU_DESCRIPTOR_HANDLE* p_rtv_cpu_descriptor_handle_;
		const D3D12_CPU_DESCRIPTOR_HANDLE* p_dsv_cpu_descriptor_handle_;
	};

	//１フレームの描画に使う情報を詰め込んだクラス
	class RenderContext
	{
		friend class argent::graphics::GraphicsEngine;
	public:
		RenderContext(const graphics::GraphicsEngine* graphics_engine, int frame_resource_index, uint32_t scene_constant_buffer_heap_index, 
			uint32_t frustum_constant_buffer_heap_index, graphics::GraphicsCommandList* graphics_command_list,
			D3D12_VIEWPORT viewport, float window_width, float window_height);

	private:
		void Begin() const;
		void End(ID3D12CommandQueue* command_queue) const;

	public:
		void SetSceneData(const SceneConstant& scene_constant) const;
		void SetFrustumData(const DirectX::XMFLOAT4X4& view_projection, const DirectX::XMFLOAT4& camera_position) const;

		RenderTarget GetRenderTargetData() const { return render_target_; }
		int GetFrameIndex() const { return frame_index_; }
		uint32_t GetSceneConstantHeapIndex() const { return scene_constant_buffer_heap_index_; }
		uint32_t GetFrustumConstantHeapIndex() const { return frustum_constant_buffer_heap_index_; }
		ID3D12GraphicsCommandList6* GetCommandList() const { return graphics_command_list_->GetCommandList(); }
		D3D12_VIEWPORT GetViewport() const { return viewport_; }
		float GetWindowWidth() const { return window_width_; }
		float GetWindowHeight() const { return window_height_; }
	private:

		const graphics::GraphicsEngine* graphics_engine_;
		int frame_index_{};
		uint32_t scene_constant_buffer_heap_index_{};
		uint32_t frustum_constant_buffer_heap_index_{};
		graphics::GraphicsCommandList* graphics_command_list_{};
		RenderTarget render_target_{};
		D3D12_VIEWPORT viewport_{};

		float window_width_;
		float window_height_;
	};
}

