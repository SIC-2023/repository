#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <algorithm>
#include <vector>

#include "../Utility/Misc.h"

namespace argent::graphics
{
	template<typename T>
	class VertexBuffer
	{
	public:
		VertexBuffer(ID3D12Device* device, const std::vector<T>& vertices, bool is_unmap = true);

		void UpdateBuffer(const T* t, int num_array)
		{
			if (is_unmap_) _ASSERT_EXPR(FALSE, L"Unmap‚³‚ê‚Ä‚¢‚é‚Ì‚Å–³Œø‚È–½—ß‚Å‚·");
			for(int i = 0; i < num_array; ++i)
			{
				mapped_resource_[i] = t[i];
			}
		}
		void SetOnCommandList(ID3D12GraphicsCommandList* command_list, UINT start_slot) const
		{
			command_list->IASetVertexBuffers(start_slot, 1u, &vertex_buffer_view_);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> vertex_buffer_;
		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;
		bool is_unmap_;
		T* mapped_resource_;
	};

	template <typename T>
	VertexBuffer<T>::VertexBuffer(ID3D12Device* device, const std::vector<T>& vertices, bool is_unmap):
		is_unmap_(is_unmap)
	{
		D3D12_HEAP_PROPERTIES heap_prop{};
		heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heap_prop.CreationNodeMask = 0u;
		heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_prop.VisibleNodeMask = 0u;

		D3D12_RESOURCE_DESC resource_desc{};
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0u;
		resource_desc.Width = sizeof(T) * vertices.size();
		resource_desc.Height = 1u;
		resource_desc.DepthOrArraySize = 1u;
		resource_desc.MipLevels = 1u;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 1u;
		resource_desc.SampleDesc.Quality = 0u;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &resource_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(vertex_buffer_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		hr = vertex_buffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_resource_));
		_ARGENT_ASSERT_EXPR(hr);


		std::copy(vertices.begin(), vertices.end(), mapped_resource_);

		if(is_unmap_)
			vertex_buffer_->Unmap(0, nullptr);

		vertex_buffer_view_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();
		vertex_buffer_view_.SizeInBytes = static_cast<UINT>(vertex_buffer_->GetDesc().Width);
		vertex_buffer_view_.StrideInBytes = sizeof(T);
	}
}