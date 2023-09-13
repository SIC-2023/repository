#pragma once
#include <d3d12.h>
#include <wrl.h>

#include "Dx12/Descriptor.h"

namespace argent::graphics
{
	struct GraphicsContext;

	class Texture
	{
	public:
		Texture(const GraphicsContext& graphics_context, const wchar_t* filename, D3D12_SRV_DIMENSION = D3D12_SRV_DIMENSION_TEXTURE2D);
		Texture(const GraphicsContext& graphics_context, const char* filename, D3D12_SRV_DIMENSION = D3D12_SRV_DIMENSION_TEXTURE2D);
		uint64_t GetHeapIndex() const { return srv_descriptor_.GetHeapIndex(); }
		float GetWidth() const { return width_; }
		float GetHeight() const { return height_; }
	private:
		HRESULT LoadTexture(ID3D12Device* device, ID3D12CommandQueue* command_queue, const wchar_t* filename);
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> texture_resource_{};
		dx12::Descriptor srv_descriptor_{};
		float width_{};
		float height_{};
	};
}

