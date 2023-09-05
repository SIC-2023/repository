#include "Texture.h"

#include <filesystem>

#include "GraphicsContext.h"
#include "GraphicsUtility.h"
#include "../Utility/Misc.h"

namespace argent::graphics
{
	Texture::Texture(const GraphicsContext& graphics_context, const wchar_t* filename):
	srv_descriptor_(graphics_context.cbv_srv_uav_heap_->PopDescriptor())
	{
		LoadTexture(graphics_context.device_, graphics_context.upload_queue_, filename);

		D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = texture_resource_->GetDesc().Format;
		shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = texture_resource_->GetDesc().MipLevels;
		graphics_context.device_->CreateShaderResourceView(texture_resource_.Get(), &shader_resource_view_desc, srv_descriptor_.GetCpuHandle());

		width_ = static_cast<float>(texture_resource_->GetDesc().Width);
		height_ = static_cast<float>(texture_resource_->GetDesc().Height);

	}

	Texture::Texture(const GraphicsContext& graphics_context, const char* filename) :
		srv_descriptor_(graphics_context.cbv_srv_uav_heap_->PopDescriptor())
	{
		std::filesystem::path filepath = filename;
		LoadTexture(graphics_context.device_, graphics_context.upload_queue_, filepath.wstring().c_str());

		D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = texture_resource_->GetDesc().Format;
		shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = texture_resource_->GetDesc().MipLevels;
		graphics_context.device_->CreateShaderResourceView(texture_resource_.Get(), &shader_resource_view_desc, srv_descriptor_.GetCpuHandle());

		width_ = static_cast<float>(texture_resource_->GetDesc().Width);
		height_ = static_cast<float>(texture_resource_->GetDesc().Height);

	}

	HRESULT Texture::LoadTexture(ID3D12Device* device, ID3D12CommandQueue* command_queue, const wchar_t* filename)
	{
		HRESULT hr = LoadTextureFromFile(device, command_queue, filename, texture_resource_.ReleaseAndGetAddressOf());
		_ARGENT_ASSERT_EXPR(hr);
		return hr;
	}
}

