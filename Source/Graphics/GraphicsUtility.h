#pragma once
#include <d3d12.h>

namespace argent::graphics
{
	HRESULT LoadTextureFromFile(ID3D12Device* device, ID3D12CommandQueue* command_queue, 
		const wchar_t* filename, ID3D12Resource** pp_resource);
}

