// ReSharper disable All
#include "DescriptorHeap.h"
#include "../../Utility/Misc.h"

namespace argent::graphics::dx12
{
	DescriptorHeap::DescriptorHeap(ID3D12Device* device, HeapType heap_type,
		uint32_t num_descriptors):
		heap_type_(heap_type)
	,	num_descriptors_(num_descriptors)
	,	current_descriptor_num_(0u)
	{
		D3D12_DESCRIPTOR_HEAP_DESC d3d_descriptor_heap_desc{};

		switch (heap_type_)
		{
		case HeapType::CbvSrvUav:
			d3d_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			heap_handle_increment_size_ = device->GetDescriptorHandleIncrementSize(d3d_descriptor_heap_desc.Type);
			d3d_descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			break;
		case HeapType::Sampler:
			d3d_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			heap_handle_increment_size_ = device->GetDescriptorHandleIncrementSize(d3d_descriptor_heap_desc.Type);
			d3d_descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			break;
		case HeapType::Rtv:
			d3d_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heap_handle_increment_size_ = device->GetDescriptorHandleIncrementSize(d3d_descriptor_heap_desc.Type);
			d3d_descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			break;
		case HeapType::Dsv:
			d3d_descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			heap_handle_increment_size_ = device->GetDescriptorHandleIncrementSize(d3d_descriptor_heap_desc.Type);
			d3d_descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			break;
		}

		d3d_descriptor_heap_desc.NumDescriptors = num_descriptors_;
		d3d_descriptor_heap_desc.NodeMask = 0u;

		const HRESULT hr = device->CreateDescriptorHeap(&d3d_descriptor_heap_desc, 
			IID_PPV_ARGS(descriptor_heap_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		cpu_handle_start_ = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
		gpu_handle_start_ = d3d_descriptor_heap_desc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ? 
			descriptor_heap_->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE();
	}

	Descriptor DescriptorHeap::PopDescriptor()
	{
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle{};
		cpu_handle.ptr = cpu_handle_start_.ptr + heap_handle_increment_size_ * current_descriptor_num_;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle{};
		gpu_handle.ptr = gpu_handle_start_.ptr + heap_handle_increment_size_ * current_descriptor_num_;
		const Descriptor descriptor = Descriptor(this, current_descriptor_num_, cpu_handle, gpu_handle);
		++current_descriptor_num_;
		return descriptor;
	}
}
