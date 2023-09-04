#pragma once

#include <d3d12.h>
#include <wrl.h>
#include "Dx12/Descriptor.h"

namespace argent::graphics
{
	class StructuredBuffer
	{
	public:
		StructuredBuffer(ID3D12Device* device, ID3D12CommandQueue* command_queue, size_t count, size_t stride, const void* p_data, const dx12::Descriptor& srv_descriptor, LPCWSTR name);
		virtual ~StructuredBuffer() = default;

		uint32_t GetHeapIndex() const { return static_cast<uint32_t>(descriptor_.GetHeapIndex()); }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> resource_{};
		dx12::Descriptor descriptor_{};
	};
}

