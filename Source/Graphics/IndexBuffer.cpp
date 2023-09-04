#include "IndexBuffer.h"

#include "../Utility/Misc.h"

namespace argent::graphics
{
	IndexBuffer::IndexBuffer(ID3D12Device* device, const std::vector<uint32_t>& indices)
	{
		D3D12_HEAP_PROPERTIES heap_prop{};
		heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_prop.CreationNodeMask = 0u;
		heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resource_desc{};
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0u;
		resource_desc.Width = sizeof(uint32_t) * indices.size();
		resource_desc.Height = 1u;
		resource_desc.DepthOrArraySize = 1u;
		resource_desc.MipLevels = 1u;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 1u;
		resource_desc.SampleDesc.Quality = 0u;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, 
			&resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(index_buffer_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		uint32_t* map{};
		hr = index_buffer_->Map(0u, nullptr, reinterpret_cast<void**>(&map));
		_ARGENT_ASSERT_EXPR(hr);

		std::ranges::copy(indices, map);

		index_buffer_->Unmap(0u, nullptr);

		index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
		index_buffer_view_.BufferLocation = index_buffer_->GetGPUVirtualAddress();
		index_buffer_view_.SizeInBytes = sizeof(uint32_t) * static_cast<uint32_t>(indices.size());
	}
}