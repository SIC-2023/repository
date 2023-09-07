#include "Texture.h"

#include <filesystem>
#include <iostream>

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "ResourceUploadBatch.h"


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
		//HRESULT hr = LoadTextureFromFile(device, command_queue, filename, texture_resource_.ReleaseAndGetAddressOf());

		HRESULT hr{ S_OK };

		std::filesystem::path dds_texture_path{ filename };
		dds_texture_path.replace_extension("DDS");

		DirectX::ResourceUploadBatch resource_upload_batch(device);
		resource_upload_batch.Begin();

		if (std::filesystem::exists(dds_texture_path.c_str()))
		{
			std::unique_ptr<uint8_t[]> dds_data;
			std::vector<D3D12_SUBRESOURCE_DATA> subresource_data;
			hr = DirectX::CreateDDSTextureFromFile(device, resource_upload_batch, dds_texture_path.wstring().c_str(),
				texture_resource_.ReleaseAndGetAddressOf());
			std::cout << dds_texture_path << std::endl;
			_ARGENT_ASSERT_EXPR(hr);
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(device, resource_upload_batch,
				filename, texture_resource_.ReleaseAndGetAddressOf());
			std::cout << dds_texture_path << std::endl;

			if(FAILED(hr))
			{
				HRESULT hr{ S_OK };

				D3D12_HEAP_PROPERTIES heapProp{};
				heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
				heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
				heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
				heapProp.VisibleNodeMask = 0;

				D3D12_RESOURCE_DESC resDesc{};
				resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				resDesc.Width = 1;
				resDesc.Height = 1;
				resDesc.DepthOrArraySize = 1;
				resDesc.SampleDesc.Count = 1;
				resDesc.SampleDesc.Quality = 0;
				resDesc.MipLevels = 1;
				resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

				hr = device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE,
					&resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr,
					IID_PPV_ARGS(texture_resource_.ReleaseAndGetAddressOf()));
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

				std::vector<unsigned int> data(1);
				auto it = data.begin();
				for (; it != data.end(); it += 1)
				{
					auto col = 0xffff7f7f;
					std::fill(it, it + 1, col);
				}

				hr = texture_resource_->WriteToSubresource(0, nullptr, data.data(),
					sizeof(unsigned int),
					static_cast<UINT>(sizeof(unsigned int) * data.size()));
				return hr;
			}
			_ARGENT_ASSERT_EXPR(hr);
		}

		const auto upload_resources_finished = resource_upload_batch.End(command_queue);
		upload_resources_finished.wait();

		return hr;

	}
}

