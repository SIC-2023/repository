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
		Texture(const GraphicsContext& graphics_context, const wchar_t* filename);
		Texture(const GraphicsContext& graphics_context, const char* filename);
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

