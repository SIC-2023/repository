#pragma once

#include <dxgi1_6.h>
#include <d3d12.h>

#include "Dx12/DescriptorHeap.h"

namespace argent::graphics
{
	struct GraphicsContext
	{
		ID3D12Device8* device_;
		IDXGISwapChain4* swap_chain_;
		ID3D12CommandQueue* upload_queue_;
		dx12::DescriptorHeap* cbv_srv_uav_heap_;
		dx12::DescriptorHeap* dsv_heap_;
		dx12::DescriptorHeap* rtv_heap_;
	};
}