#pragma once
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl.h>

#include <memory>

#include "Dx12/Descriptor.h"
#include "ConstantBuffer.h"

#include "../../Assets/Shader/Constant.h"

namespace argent::graphics
{
	//スワップチェーンへの出力を担うクラス
	//最終的な描画はこのクラスが持つレンダーターゲットに行う
	class FrameResource
	{
	public:
		FrameResource(ID3D12Device* device, IDXGISwapChain* swap_chain, uint32_t back_buffer_index, 
			dx12::Descriptor rtv_descriptor, dx12::Descriptor dsv_descriptor, dx12::Descriptor scene_cbv_descriptor, dx12::Descriptor frustum_cbv_descriptor);

		//バックバッファへの描画開始
		void Begin(ID3D12GraphicsCommandList* command_list, 
			const D3D12_VIEWPORT& viewport, const D3D12_RECT& rect,
			const float clear_color[4]) const;

		//バックバッファへの描画終了
		void End(ID3D12GraphicsCommandList* command_list) const;

		void UpdateSceneConstantBuffer(const SceneConstant& scene_constant) const { scene_constant_buffer_->UpdateConstantBuffer(scene_constant); }
		void UpdateFrustumConstantBuffer(const DirectX::XMFLOAT4X4& view_projection_matrix, const DirectX::XMFLOAT4& camera_position) const;
		uint64_t GetSceneConstantHeapIndex() const { return scene_constant_buffer_->GetHeapIndex(); }
		uint64_t GetFrustumConstantHeapIndex() const { return frustum_constant_buffer_->GetHeapIndex(); }

	private:

		Microsoft::WRL::ComPtr<ID3D12Resource> back_buffer_;
		Microsoft::WRL::ComPtr<ID3D12Resource> depth_stencil_resource_;
		dx12::Descriptor rtv_descriptor_;
		dx12::Descriptor dsv_descriptor_;
		D3D12_CPU_DESCRIPTOR_HANDLE rtv_cpu_descriptor_handle_;
		D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu_descriptor_handle_;
		std::unique_ptr<ConstantBuffer<SceneConstant>> scene_constant_buffer_;
		std::unique_ptr<ConstantBuffer<Frustum>> frustum_constant_buffer_;
	};
}

