#pragma once
#include <d3d12.h>

#include <cstdint>

namespace argent::graphics::dx12
{
	class DescriptorHeap;

	class Descriptor final
	{
	public:
		friend class DescriptorHeap;
	public:
		Descriptor() = default;
		Descriptor(const Descriptor&) = default;

	private:
		Descriptor(DescriptorHeap* descriptor_heap, uint64_t heap_index, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) :
			heap_index_(heap_index)
			, descriptor_heap_(descriptor_heap)
			, cpu_handle_(cpu_handle)
			, gpu_handle_(gpu_handle)
		{}

	public:
		~Descriptor() = default;

		DescriptorHeap* GetDescriptorHeap() const { return descriptor_heap_; }
		uint64_t GetHeapIndex() const { return heap_index_; }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const { return cpu_handle_; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const { return gpu_handle_; }
	private:
		uint64_t heap_index_;
		DescriptorHeap* descriptor_heap_;
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_;
	};
}