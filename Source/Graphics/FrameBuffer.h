#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "GraphicsContext.h"
#include "Dx12/Descriptor.h"

namespace argent::graphics
{
	//TODO マルチレンダーターゲットに対応する
	class FrameBuffer
	{
	public:
		FrameBuffer(const GraphicsContext& graphics_context,
			UINT64 width, UINT height, DXGI_FORMAT format);
		virtual ~FrameBuffer() = default;

		virtual void Begin(ID3D12GraphicsCommandList* command_list, UINT num_rects = 0u, const D3D12_RECT* p_rect = nullptr);
		void End(ID3D12GraphicsCommandList* command_list);

		uint64_t GetSrvHeapIndex() const { return srv_descriptor_.GetHeapIndex(); }
	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> sr_rt_resource_;
		Microsoft::WRL::ComPtr<ID3D12Resource> ds_resource_;
		dx12::Descriptor rtv_descriptor_;
		dx12::Descriptor srv_descriptor_;
		dx12::Descriptor dsv_descriptor_;


		float clear_color_[4] { 0, 0, 1, 1 };
	};
}

