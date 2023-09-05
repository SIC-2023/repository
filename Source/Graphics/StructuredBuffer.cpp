#include "StructuredBuffer.h"

#include "ResourceUploadBatch.h"

#include "../Utility/Misc.h"

namespace argent::graphics
{
	StructuredBuffer::StructuredBuffer(ID3D12Device* device, ID3D12CommandQueue* command_queue, size_t count, size_t stride, const void* p_data, const dx12::Descriptor& srv_descriptor, LPCWSTR name):
		descriptor_(srv_descriptor)
	{
        D3D12_HEAP_PROPERTIES heap_prop = {};
        heap_prop.Type = D3D12_HEAP_TYPE_DEFAULT;
        heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heap_prop.CreationNodeMask = 1;
        heap_prop.VisibleNodeMask = 1;

        auto size = count * stride;

        // リソースの設定.
        D3D12_RESOURCE_DESC resource_desc = {};
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resource_desc.Alignment = 0;
        resource_desc.Width = size;
        resource_desc.Height = 1;
        resource_desc.DepthOrArraySize = 1;
        resource_desc.MipLevels = 1;
        resource_desc.Format = DXGI_FORMAT_UNKNOWN;
        resource_desc.SampleDesc.Count = 1;
        resource_desc.SampleDesc.Quality = 0;
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

        HRESULT hr = device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE,
            &resource_desc, D3D12_RESOURCE_STATE_COMMON /*D3D12_RESOURCE_STATE_COPY_DEST*/ /*サンプルではCopyだが警告が出るのでCOMMONにする*/,
            nullptr, IID_PPV_ARGS(resource_.ReleaseAndGetAddressOf()));
        _ARGENT_ASSERT_EXPR(hr);

        D3D12_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
        shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
        shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shader_resource_view_desc.Buffer.FirstElement = 0;
        shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(count);
        shader_resource_view_desc.Buffer.StructureByteStride = static_cast<UINT>(stride);
        shader_resource_view_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(resource_.Get(), &shader_resource_view_desc, descriptor_.GetCpuHandle());

        //リソースのアップロード
        //リソースをアップロードしないと使えない
        D3D12_SUBRESOURCE_DATA subresource_data{};
        subresource_data.pData = p_data;
        subresource_data.RowPitch = size;
        subresource_data.SlicePitch = size;

        //DirectXTKのresourceUploadBatch
        DirectX::ResourceUploadBatch resource_upload_batch(device);

        resource_upload_batch.Begin();

        resource_upload_batch.Upload(resource_.Get(), 0u, &subresource_data, 1u);

        auto wait = resource_upload_batch.End(command_queue);
        wait.wait();

        resource_->SetName(name);
	}
}
