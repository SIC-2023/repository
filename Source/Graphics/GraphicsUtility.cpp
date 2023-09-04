#include "GraphicsUtility.h"

#include <filesystem>
#include <iostream>

#include "../../External/DirectXTK12/Inc/DDSTextureLoader.h"
#include "../../External/DirectXTK12/Inc/WICTextureLoader.h"
#include "../../External/DirectXTK12/Inc/ResourceUploadBatch.h"

#include "../Utility/Misc.h"

namespace argent::graphics
{
	HRESULT LoadTextureFromFile(ID3D12Device* device, ID3D12CommandQueue* command_queue, const wchar_t* filename,
		ID3D12Resource** pp_resource)
	{
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
				pp_resource);
			std::cout << dds_texture_path << std::endl;
			_ARGENT_ASSERT_EXPR(hr);
		}
		else
		{
			hr = DirectX::CreateWICTextureFromFile(device, resource_upload_batch,
				filename, pp_resource);
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
					IID_PPV_ARGS(pp_resource));
				_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));

				std::vector<unsigned int> data(1);
				auto it = data.begin();
				for (; it != data.end(); it += 1)
				{
					auto col = 0xffff7f7f;
					std::fill(it, it + 1, col);
				}

				hr = (*pp_resource)->WriteToSubresource(0, nullptr, data.data(),
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
