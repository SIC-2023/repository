#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "Dx12/Descriptor.h"
#include "../Utility/Misc.h"

namespace argent::graphics
{
	template<class T>
	class ConstantBuffer
	{
	public:
		ConstantBuffer(ID3D12Device* device, const dx12::Descriptor& descriptor,const T* fill_value = nullptr);
		~ConstantBuffer()
		{
			constant_buffer_->Unmap(0, nullptr);
		}

		void UpdateConstantBuffer(const T& t) { *mapped_resource_ = t; }

		uint64_t GetHeapIndex() const { return descriptor_.GetHeapIndex(); }

	private:
		static size_t CalcAlignmentSize(size_t s)
		{
			return (s + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> constant_buffer_;
		dx12::Descriptor descriptor_;
		T* mapped_resource_;
	};

	template <class T>
	ConstantBuffer<T>::ConstantBuffer(ID3D12Device* device, const dx12::Descriptor& descriptor,const T* fill_value):
		descriptor_(descriptor)
	{
		HRESULT hr{ S_OK };
		D3D12_HEAP_PROPERTIES heap_prop{};
		heap_prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heap_prop.CreationNodeMask = 0u;
		heap_prop.VisibleNodeMask = 0u;

		D3D12_RESOURCE_DESC resource_desc{};
		resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resource_desc.Alignment = 0u;
		resource_desc.Width = CalcAlignmentSize(sizeof(T));
		resource_desc.Height = 1u;
		resource_desc.DepthOrArraySize = 1u;
		resource_desc.MipLevels = 1u;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 1u;
		resource_desc.SampleDesc.Quality = 0u;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		hr = device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(constant_buffer_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		
		hr = constant_buffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_resource_));
		_ARGENT_ASSERT_EXPR(hr);

		if(fill_value) *mapped_resource_ = *fill_value;

		constant_buffer_->Unmap(0, nullptr);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_desc{};
		cbv_desc.SizeInBytes = static_cast<UINT>(constant_buffer_->GetDesc().Width);
		cbv_desc.BufferLocation = constant_buffer_->GetGPUVirtualAddress();

		device->CreateConstantBufferView(&cbv_desc, descriptor.GetCpuHandle());
		constant_buffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped_resource_));

	}
}