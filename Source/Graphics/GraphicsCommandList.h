#pragma once

#include <d3d12.h>
#include <wrl.h>

namespace argent::graphics
{
	class GraphicsCommandList
	{
	public:
		GraphicsCommandList(ID3D12Device* device);
		virtual ~GraphicsCommandList() = default;

		void Begin();


		void Close();
		void Reset();

		ID3D12GraphicsCommandList6* GetCommandList() const { return command_list_.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> command_list_;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator_;
		bool is_closed_;
		bool is_reset_;
	};
}

