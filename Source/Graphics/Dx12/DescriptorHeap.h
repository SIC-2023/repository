#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <cstdint>

#include "Descriptor.h"

namespace argent::graphics::dx12
{
	//d3d12のDescriptorHeapを保持するクラス
	//PopDescriptor()で新しいDescriptorを取得
	class DescriptorHeap final
	{
	public:
		enum class HeapType{ CbvSrvUav, Sampler, Rtv, Dsv, Count };

	public:
		DescriptorHeap(ID3D12Device* device, HeapType heap_type, uint32_t num_descriptors);

		DescriptorHeap(const DescriptorHeap&) = delete;
		DescriptorHeap(const DescriptorHeap&&) = delete;
		DescriptorHeap& operator=(const DescriptorHeap&) = delete;
		DescriptorHeap& operator=(const DescriptorHeap&&) = delete;

		~DescriptorHeap() = default;

		ID3D12DescriptorHeap* GetD3d12DescriptorHeap() const { return descriptor_heap_.Get(); }

		/**
		 * \brief 新しいディスクリプタを作成
		 * \return ディスクリプタ
		 */
		Descriptor PopDescriptor();
		uint32_t GetCurrentDescriptorNum() const { return current_descriptor_num_; }
		uint32_t GetIncrementSize() const { return heap_handle_increment_size_; }
		size_t GetGpuHandleStart() const { return gpu_handle_start_.ptr; }
		size_t GetCpuHandleStart() const { return cpu_handle_start_.ptr; }
	private:

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor_heap_;
		HeapType heap_type_;
		uint32_t num_descriptors_;
		uint32_t current_descriptor_num_;
		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_start_;
		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_start_;
		uint32_t heap_handle_increment_size_;
	};
}