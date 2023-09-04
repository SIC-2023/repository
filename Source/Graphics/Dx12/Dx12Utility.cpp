#include "Dx12Utility.h"
#include "../../Utility/Misc.h"

namespace argent::graphics::dx12
{
	//深度ステンシルバッファを作成
	HRESULT CreateDepthStencilBufferAndView(ID3D12Device* device, UINT64 width, UINT height,
		ID3D12Resource** pp_depth_stencil_resource, D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle)
	{
		HRESULT hr{ S_OK };
		D3D12_RESOURCE_DESC res_desc{};
		res_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		res_desc.Width = width;
		res_desc.Height = height;
		res_desc.DepthOrArraySize = 1;
		res_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		res_desc.SampleDesc.Count = 1u;
		res_desc.SampleDesc.Quality = 0u;
		res_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_HEAP_PROPERTIES heap_prop{};
		heap_prop.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_CLEAR_VALUE depth_clear_value{};
		depth_clear_value.DepthStencil.Depth = 1.0f;
		depth_clear_value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		hr = device->CreateCommittedResource(&heap_prop, D3D12_HEAP_FLAG_NONE,
			&res_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depth_clear_value,
			IID_PPV_ARGS(pp_depth_stencil_resource));
		_ARGENT_ASSERT_EXPR(hr);

		D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.Flags = D3D12_DSV_FLAG_NONE;
		depth_stencil_view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		device->CreateDepthStencilView(*pp_depth_stencil_resource, &depth_stencil_view_desc,
			cpu_descriptor_handle);

		return hr;
	}

	//トランジションリソースバリアを張る
	void SetTransitionResourceBarrier(ID3D12GraphicsCommandList* command_list, 
		D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after, 
		ID3D12Resource* resource)
	{
		D3D12_RESOURCE_BARRIER resource_barrier{};
		resource_barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		resource_barrier.Transition.StateBefore = state_before;
		resource_barrier.Transition.StateAfter = state_after;
		resource_barrier.Transition.Subresource = 0u;
		resource_barrier.Transition.pResource = resource;
		command_list->ResourceBarrier(1u, &resource_barrier);
	}

	//---------------------------------ブレンドモード---------------------------------//

	D3D12_RENDER_TARGET_BLEND_DESC GenerateRenderTargetBlendDesc(BlendMode blend_mode)
	{
		D3D12_RENDER_TARGET_BLEND_DESC render_target_blend_desc{};
		render_target_blend_desc.LogicOpEnable = FALSE;
		render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		render_target_blend_desc.BlendEnable = TRUE;

		switch (blend_mode)
		{
		case BlendMode::Alpha:
			render_target_blend_desc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			render_target_blend_desc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			render_target_blend_desc.BlendOp = D3D12_BLEND_OP_ADD;
			render_target_blend_desc.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
			render_target_blend_desc.DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
			render_target_blend_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		case BlendMode::Add:
			render_target_blend_desc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			render_target_blend_desc.DestBlend = D3D12_BLEND_ONE;
			render_target_blend_desc.BlendOp = D3D12_BLEND_OP_ADD;
			render_target_blend_desc.SrcBlendAlpha = D3D12_BLEND_ONE;
			render_target_blend_desc.DestBlendAlpha = D3D12_BLEND_ZERO;
			render_target_blend_desc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		}

		return render_target_blend_desc;
	}

	D3D12_BLEND_DESC GetBlendDesc(BlendMode blend_mode, bool alpha_to_coverage_enable, bool independent_blend_enable)
	{
		D3D12_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = alpha_to_coverage_enable;
		blend_desc.IndependentBlendEnable = independent_blend_enable;
		blend_desc.RenderTarget[0] = GenerateRenderTargetBlendDesc(blend_mode);
		return blend_desc;
	}

	D3D12_BLEND_DESC GetBlendDesc(UINT num_render_targets, const BlendMode blend_mode[],
		bool alpha_to_coverage_enable, bool independent_blend_enable)
	{
		if (num_render_targets > 8) _ASSERT_EXPR(FALSE, L"レンダーターゲットが最大数を超えています");
		D3D12_BLEND_DESC blend_desc{};

		blend_desc.AlphaToCoverageEnable = alpha_to_coverage_enable;
		blend_desc.IndependentBlendEnable = independent_blend_enable;

		for (UINT i = 0; i < num_render_targets; ++i)
		{
			blend_desc.RenderTarget[i] = GenerateRenderTargetBlendDesc(blend_mode[i]);
		}

		return blend_desc;
	}

	//---------------------------------ラスタライザステート---------------------------------//

	D3D12_RASTERIZER_DESC GetRasterizerDesc(RasterizerMode rasterizer_mode, bool front_counter_clockwise)
	{
		D3D12_RASTERIZER_DESC rasterizer_desc{};

		switch (rasterizer_mode)
		{
		case RasterizerMode::CullNoneSolid:
			rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
			rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
			break;
		case RasterizerMode::CullNoneWireFrame:
			rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
			rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
			break;
		case RasterizerMode::CullBackSolid:
			rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
			rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
			break;
		case RasterizerMode::CullBackWireFrame:
			rasterizer_desc.CullMode = D3D12_CULL_MODE_BACK;
			rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
			break;
		case RasterizerMode::CullFrontSolid:
			rasterizer_desc.CullMode = D3D12_CULL_MODE_FRONT;
			rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
			break;
		case RasterizerMode::CullFrontWireFrame:
			rasterizer_desc.CullMode = D3D12_CULL_MODE_FRONT;
			rasterizer_desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
			break;
		}

		rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		rasterizer_desc.ForcedSampleCount = 0;
		rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		rasterizer_desc.FrontCounterClockwise = front_counter_clockwise;

		return rasterizer_desc;
	}

	//---------------------------------デプスステンシルステート---------------------------------//

	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(DepthMode depth_mode)
	{
		D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};

		depth_stencil_desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		switch (depth_mode)
		{
		case DepthMode::TestOnWriteOn:
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			break;
		case DepthMode::TestOnWriteOff:
			depth_stencil_desc.DepthEnable = TRUE;
			depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			break;
		case DepthMode::TestOffWriteOn:
			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			break;
		case DepthMode::TestOffWriteOff:
			depth_stencil_desc.DepthEnable = FALSE;
			depth_stencil_desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			break;
		}


		depth_stencil_desc.StencilEnable = false;
        depth_stencil_desc.FrontFace.StencilFailOp = 
			depth_stencil_desc.FrontFace.StencilDepthFailOp = 
			depth_stencil_desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        depth_stencil_desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        depth_stencil_desc.BackFace = depth_stencil_desc.FrontFace;

		return depth_stencil_desc;
	}

	//---------------------------------シェーダーリフレクション---------------------------------//
	DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask)
	{
		DXGI_FORMAT ret = DXGI_FORMAT_UNKNOWN;
		if (mask == 0x01) //1
		{
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				ret = DXGI_FORMAT_R32_FLOAT;
				break;
			case D3D_REGISTER_COMPONENT_UINT32:
				ret = DXGI_FORMAT_R32_UINT;
				break;

			}
		}
		else if (mask == 0x03)//2
		{
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				ret = DXGI_FORMAT_R32G32_FLOAT;
				break;
			case D3D_REGISTER_COMPONENT_UINT32:
				ret = DXGI_FORMAT_R32G32_UINT;
				break;
			}
		}
		else if (mask == 0x07)//3
		{
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				ret = DXGI_FORMAT_R32G32B32_FLOAT;
				break;
			case D3D_REGISTER_COMPONENT_UINT32:
				ret = DXGI_FORMAT_R32G32B32_UINT;
				break;
			}
		}
		else if (mask == 0x0f)//4
		{
			switch (type)
			{
			case D3D_REGISTER_COMPONENT_FLOAT32:
				ret = DXGI_FORMAT_R32G32B32A32_FLOAT;
				break;
			case D3D_REGISTER_COMPONENT_UINT32:
				ret = DXGI_FORMAT_R32G32B32A32_UINT;
				break;
			}
		}

		return ret;
	}


}
