#include "GraphicsCommandList.h"

#include "../Utility/Misc.h"

namespace argent::graphics
{
	GraphicsCommandList::GraphicsCommandList(ID3D12Device* device):
		is_closed_(false)
	,	is_reset_(false)
	{
		HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, 
			IID_PPV_ARGS(command_allocator_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		hr = device->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT,
			command_allocator_.Get(), nullptr, 
			IID_PPV_ARGS(command_list_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		Close();
		Reset();
	}

	void GraphicsCommandList::Begin()
	{
		Reset();
		is_reset_ = false;
		is_closed_ = false;
	}

	void GraphicsCommandList::Close()
	{
		if (is_closed_) return;
		command_list_->Close();
		is_closed_ = true;
	}

	void GraphicsCommandList::Reset()
	{
		if (is_reset_) return;

		command_allocator_.Get()->Reset();
		command_list_.Get()->Reset(command_allocator_.Get(), nullptr);
		is_reset_ = true;
	}
}
