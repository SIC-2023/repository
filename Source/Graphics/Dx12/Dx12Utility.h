#pragma once
#include <d3d12.h>

namespace argent::graphics::dx12
{
	/**
	 * \brief デプスステンシルバッファとビューを作成する
	 * \param device d3d12のデバイス
	 * \param width バッファの幅
	 * \param height バッファの高さ
	 * \param pp_depth_stencil_resource 作成したリソースを格納する 
	 * \param cpu_descriptor_handle リソースのcpuハンドル
	 * \return 成功したかどうか
	 */
	HRESULT CreateDepthStencilBufferAndView(ID3D12Device* device, UINT64 width, UINT height,
	                                        ID3D12Resource** pp_depth_stencil_resource, D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor_handle);

	//todo 置く場所間違ってると思う
	/**
	 * \brief トランジションリソースバリアを張る
	 * \param command_list 命令を乗せるコマンドリスト
	 * \param state_before 現在のリソースステート
	 * \param state_after 変更後のリソースステート
	 * \param resource 対象のリソース
	 */
	void SetTransitionResourceBarrier(ID3D12GraphicsCommandList* command_list,
	                                  D3D12_RESOURCE_STATES state_before, D3D12_RESOURCE_STATES state_after,
	                                  ID3D12Resource* resource);

	//---------------------------------ブレンドモード---------------------------------//

	enum class BlendMode { Alpha, Add };
	D3D12_BLEND_DESC GetBlendDesc(BlendMode blend_mode, bool alpha_to_coverage_enable = false, bool independent_blend_enable = false);

	/**
	 * \brief ブレンドデスクを作成
	 * \param num_render_targets レンダーターゲットの数
	 * \param blend_mode レンダーターゲットの数だけ必要
	 * \param alpha_to_coverage_enable [in]
	 * \param independent_blend_enable [in]
	 * \return D3D12_BLEND_DESC
	 */
	D3D12_BLEND_DESC GetBlendDesc(UINT num_render_targets, const BlendMode blend_mode[],
		bool alpha_to_coverage_enable = false, bool independent_blend_enable = false);

	//---------------------------------ラスタライザステート---------------------------------//

	enum class RasterizerMode { CullNoneSolid, CullNoneWireFrame, CullBackSolid,
		CullBackWireFrame, CullFrontSolid, CullFrontWireFrame };
	D3D12_RASTERIZER_DESC GetRasterizerDesc(RasterizerMode rasterizer_mode, bool front_counter_clockwise = true);

	//---------------------------------デプスステンシルステート---------------------------------//

	enum class DepthMode { TestOnWriteOn, TestOnWriteOff, TestOffWriteOn, TestOffWriteOff };
	D3D12_DEPTH_STENCIL_DESC GetDepthStencilDesc(DepthMode depth_mode);


	//---------------------------------シェーダーリフレクション---------------------------------//

	DXGI_FORMAT GetDxgiFormat(D3D_REGISTER_COMPONENT_TYPE type, BYTE mask);


}