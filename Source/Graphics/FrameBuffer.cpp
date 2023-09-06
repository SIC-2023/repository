#include "FrameBuffer.h"

#include "d3dx12.h"

#include "../Utility/Misc.h"

namespace argent::graphics
{
	FrameBuffer::FrameBuffer(const GraphicsContext& graphics_context, UINT64 width, UINT height, DXGI_FORMAT format):
		rtv_descriptor_(graphics_context.rtv_heap_->PopDescriptor())
	,	srv_descriptor_(graphics_context.cbv_srv_uav_heap_->PopDescriptor())
	,	dsv_descriptor_(graphics_context.dsv_heap_->PopDescriptor())
	{
		const D3D12_HEAP_PROPERTIES heap_prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

		HRESULT hr{ S_OK };

		//Create Resource, RTV and SRV
		{
			// Resource
			{
				D3D12_RESOURCE_DESC resource_desc{};
				resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				resource_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
				resource_desc.Width = width;
				resource_desc.Height = height;
				resource_desc.DepthOrArraySize = 1u;
				resource_desc.MipLevels = 1u;
				resource_desc.Format = format;
				resource_desc.SampleDesc.Count = 1u;
				resource_desc.SampleDesc.Quality = 0u;
				resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
				resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

				const D3D12_CLEAR_VALUE clear_value = CD3DX12_CLEAR_VALUE(resource_desc.Format, clear_color_);

				hr = graphics_context.device_->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
					&clear_value, IID_PPV_ARGS(sr_rt_resource_.ReleaseAndGetAddressOf()));
				_ARGENT_ASSERT_EXPR(hr);
			}

			//RTV and SRV
			{
				D3D12_RENDER_TARGET_VIEW_DESC rtv_desc{};
				rtv_desc.Format = sr_rt_resource_->GetDesc().Format;
				rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

				graphics_context.device_->CreateRenderTargetView(sr_rt_resource_.Get(), &rtv_desc, rtv_descriptor_.GetCpuHandle());

				D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc{};
				srv_desc.Format = sr_rt_resource_->GetDesc().Format;
				srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srv_desc.Texture2D.MipLevels = 1u;

				graphics_context.device_->CreateShaderResourceView(sr_rt_resource_.Get(), &srv_desc, srv_descriptor_.GetCpuHandle());
			}
		}

		//Create Depth Resource and DSV
		{
			const D3D12_CLEAR_VALUE clear_value = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D24_UNORM_S8_UINT, 1.0f, 0.0f);

			D3D12_RESOURCE_DESC resource_desc{};
			resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			resource_desc.Width = width;
			resource_desc.Height = height;
			resource_desc.DepthOrArraySize = 1u;
			resource_desc.SampleDesc.Count = 1u;
			resource_desc.SampleDesc.Quality = 0u;
			resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			resource_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

			hr = graphics_context.device_->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE, &resource_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_value,
				IID_PPV_ARGS(ds_resource_.ReleaseAndGetAddressOf()));
			_ARGENT_ASSERT_EXPR(hr);

			D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
			dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
			dsv_desc.Format = ds_resource_->GetDesc().Format;
			dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			graphics_context.device_->CreateDepthStencilView(ds_resource_.Get(), &dsv_desc, dsv_descriptor_.GetCpuHandle());
		}
	}

	void FrameBuffer::Begin(ID3D12GraphicsCommandList* command_list, 
							UINT num_rects, const D3D12_RECT* p_rect)
	{
		//TODO
		//rtv dsv クリア
		command_list->ClearRenderTargetView(rtv_descriptor_.GetCpuHandle(), 
			clear_color_, num_rects, p_rect);
		command_list->ClearDepthStencilView(dsv_descriptor_.GetCpuHandle(), 
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 
			0u, 0u, nullptr);

		//リソースバリア
		const D3D12_RESOURCE_BARRIER resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			sr_rt_resource_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, 
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		command_list->ResourceBarrier(1u, &resource_barrier);

		//レンダーターゲットとしてセット
		const auto rtv_cpu_handle = rtv_descriptor_.GetCpuHandle();
		const auto dsv_cpu_handle = dsv_descriptor_.GetCpuHandle();
		command_list->OMSetRenderTargets(1u, &rtv_cpu_handle, 
			false, &dsv_cpu_handle);
	}

	void FrameBuffer::End(ID3D12GraphicsCommandList* command_list)
	{
		//リソースバリア
		const D3D12_RESOURCE_BARRIER resource_barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			sr_rt_resource_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, 
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		command_list->ResourceBarrier(1u, &resource_barrier);

		//??? 元々セットされてたやつをセットし直す or nullレンダーターゲットをセットする
	}

}
