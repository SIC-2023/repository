#include "FrameResource.h"

#include "Dx12/Dx12Utility.h"
//#include "GraphicsEngine.h"
#include "../Utility/Misc.h"

namespace argent::graphics
{
	FrameResource::FrameResource(ID3D12Device* device, IDXGISwapChain* swap_chain, uint32_t back_buffer_index,
		dx12::Descriptor rtv_descriptor, dx12::Descriptor dsv_descriptor, dx12::Descriptor scene_cbv_descriptor,
		dx12::Descriptor frustum_cbv_descriptor):
		rtv_descriptor_(rtv_descriptor)
		, dsv_descriptor_(dsv_descriptor)
		, rtv_cpu_descriptor_handle_(rtv_descriptor.GetCpuHandle())
		, dsv_cpu_descriptor_handle_(dsv_descriptor.GetCpuHandle())
	{
		HRESULT hr = swap_chain->GetBuffer(back_buffer_index, IID_PPV_ARGS(back_buffer_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		device->CreateRenderTargetView(back_buffer_.Get(), nullptr, rtv_descriptor_.GetCpuHandle());

		hr = dx12::CreateDepthStencilBufferAndView(device, back_buffer_->GetDesc().Width, back_buffer_->GetDesc().Height,
			depth_stencil_resource_.ReleaseAndGetAddressOf(), dsv_descriptor_.GetCpuHandle());
		_ARGENT_ASSERT_EXPR(hr);

		scene_constant_buffer_ = std::make_unique<ConstantBuffer<SceneConstant>>(device, scene_cbv_descriptor);
		frustum_constant_buffer_ = std::make_unique<ConstantBuffer<Frustum>>(device, frustum_cbv_descriptor);
	}

	FrameResource::~FrameResource()
	{
	}

	//•`‰æŠJŽn
	void FrameResource::Begin(ID3D12GraphicsCommandList* command_list, const D3D12_VIEWPORT& viewport,
		const D3D12_RECT& rect, const float clear_color[4]) const
	{
		dx12::SetTransitionResourceBarrier(command_list, D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET, back_buffer_.Get());

		command_list->OMSetRenderTargets(1u, &rtv_cpu_descriptor_handle_, false,
			&dsv_cpu_descriptor_handle_);
		command_list->ClearRenderTargetView(rtv_cpu_descriptor_handle_, clear_color, 0u, nullptr);
		command_list->ClearDepthStencilView(dsv_cpu_descriptor_handle_, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.0f, 0u, 0u, nullptr);
		command_list->RSSetViewports(1u, &viewport);
		command_list->RSSetScissorRects(1u, &rect);
	}

	void FrameResource::End(ID3D12GraphicsCommandList* command_list) const
	{
		dx12::SetTransitionResourceBarrier(command_list, D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT, back_buffer_.Get());
	}

	void FrameResource::UpdateFrustumConstantBuffer(const DirectX::XMFLOAT4X4& view_projection_matrix,
		const DirectX::XMFLOAT4& camera_position) const
	{
		using namespace DirectX;

		Frustum tmp_frustum{};
		// Left Frustum Plane
		// Add first column of the matrix to the fourth column
		tmp_frustum.frustum_panels_[0].x = view_projection_matrix._14 + view_projection_matrix._11;
		tmp_frustum.frustum_panels_[0].y = view_projection_matrix._24 + view_projection_matrix._21;
		tmp_frustum.frustum_panels_[0].z = view_projection_matrix._34 + view_projection_matrix._31;
		tmp_frustum.frustum_panels_[0].w = view_projection_matrix._44 + view_projection_matrix._41;

		// Right Frustum Plane
		// Subtract first column of matrix from the fourth column
		tmp_frustum.frustum_panels_[1].x = view_projection_matrix._14 - view_projection_matrix._11;
		tmp_frustum.frustum_panels_[1].y = view_projection_matrix._24 - view_projection_matrix._21;
		tmp_frustum.frustum_panels_[1].z = view_projection_matrix._34 - view_projection_matrix._31;
		tmp_frustum.frustum_panels_[1].w = view_projection_matrix._44 - view_projection_matrix._41;

		// Top Frustum Plane
		// Subtract second column of matrix from the fourth column
		tmp_frustum.frustum_panels_[2].x = view_projection_matrix._14 - view_projection_matrix._12;
		tmp_frustum.frustum_panels_[2].y = view_projection_matrix._24 - view_projection_matrix._22;
		tmp_frustum.frustum_panels_[2].z = view_projection_matrix._34 - view_projection_matrix._32;
		tmp_frustum.frustum_panels_[2].w = view_projection_matrix._44 - view_projection_matrix._42;

		// Bottom Frustum Plane
		// Add second column of the matrix to the fourth column
		tmp_frustum.frustum_panels_[3].x = view_projection_matrix._14 + view_projection_matrix._12;
		tmp_frustum.frustum_panels_[3].y = view_projection_matrix._24 + view_projection_matrix._22;
		tmp_frustum.frustum_panels_[3].z = view_projection_matrix._34 + view_projection_matrix._32;
		tmp_frustum.frustum_panels_[3].w = view_projection_matrix._44 + view_projection_matrix._42;

		// Near Frustum Plane
		// We could add the third column to the fourth column to get the near plane,
		// but we don't have to do this because the third column IS the near plane
		tmp_frustum.frustum_panels_[4].x = view_projection_matrix._13;
		tmp_frustum.frustum_panels_[4].y = view_projection_matrix._23;
		tmp_frustum.frustum_panels_[4].z = view_projection_matrix._33;
		tmp_frustum.frustum_panels_[4].w = view_projection_matrix._43;

		// Far Frustum Plane
		// Subtract third column of matrix from the fourth column
		tmp_frustum.frustum_panels_[5].x = view_projection_matrix._14 - view_projection_matrix._13;
		tmp_frustum.frustum_panels_[5].y = view_projection_matrix._24 - view_projection_matrix._23;
		tmp_frustum.frustum_panels_[5].z = view_projection_matrix._34 - view_projection_matrix._33;
		tmp_frustum.frustum_panels_[5].w = view_projection_matrix._44 - view_projection_matrix._43;

		// Normalize plane normals (A, B and C (xyz))
		// Also take note that planes face inward
		for (int i = 0; i < 6; ++i)
		{
			float length = sqrt((tmp_frustum.frustum_panels_[i].x * tmp_frustum.frustum_panels_[i].x) + (tmp_frustum.frustum_panels_[i].y * tmp_frustum.frustum_panels_[i].y) + (tmp_frustum.frustum_panels_[i].z * tmp_frustum.frustum_panels_[i].z));
			tmp_frustum.frustum_panels_[i].x /= length;
			tmp_frustum.frustum_panels_[i].y /= length;
			tmp_frustum.frustum_panels_[i].z /= length;
			tmp_frustum.frustum_panels_[i].w /= length;
		}

		tmp_frustum.eye_position_ = camera_position;

		frustum_constant_buffer_->UpdateConstantBuffer(tmp_frustum);
	}
}
