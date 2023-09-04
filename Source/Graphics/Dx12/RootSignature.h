#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace argent::graphics::dx12
{
	class RootSignature;

	RootSignature* GetUniqueRootSignature();

	class RootSignature
	{
		static HRESULT CreateUniqueRootSignature();
		
	};
}

