#pragma once
#include <d3d12.h>
#include <wrl.h>

#include <memory>
#include <unordered_map>

#include "Shader.h"
#include "Dx12/Dx12Utility.h"

namespace argent::graphics
{
	//通常の頂点シェーダーを使用した描画パイプラインを作成するときに使う
	struct GraphicsPipelineDesc
	{
		const wchar_t* vs_filename_ = nullptr;	//必ず値をセットすること
		const wchar_t* ps_filename_ = nullptr;	//nullptrでもOk、ただしそのときはnum_render_targetsは0にすること
		const wchar_t* gs_filename_ = nullptr;	//nullptrでもOk
		const wchar_t* ds_filename_ = nullptr;	//nullptrでもOk
		const wchar_t* hs_filename_ = nullptr;	//nullptrでもOk
		UINT num_render_targets_ = 1u;
		dx12::BlendMode blend_mode_[8] = { dx12::BlendMode::Alpha };
		DXGI_FORMAT	rtv_format_[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
		bool alpha_to_coverage_enable_ = false;
		bool independent_blend_enable_ = false;
		dx12::DepthMode depth_mode_ = dx12::DepthMode::TestOnWriteOn;
		dx12::RasterizerMode rasterizer_mode_ = dx12::RasterizerMode::CullNoneSolid;	
		DXGI_FORMAT dsv_format_ = DXGI_FORMAT_D24_UNORM_S8_UINT;
		D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type_ = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		bool independent_input_layout_buffer_enable_ = true;
		bool front_counter_clockwise_ = true;
	};

	//メッシュシェーダーを使用した描画パイプラインを作成するときに使う
	struct MeshShaderPipelineDesc
	{
		const wchar_t* as_filename_ = nullptr;	//Amplification Shader [Optional]
		const wchar_t* ms_filename_ = nullptr;	//Mesh Shader [Neccesary]
		const wchar_t* ps_filename_ = nullptr;	//Pixel Shader [Neccesary]
		UINT num_render_targets_ = 1u;
		dx12::BlendMode blend_mode_[8] = { dx12::BlendMode::Alpha };
		DXGI_FORMAT	rtv_format_[8] = { DXGI_FORMAT_R8G8B8A8_UNORM };
		bool alpha_to_coverage_enable_ = false;
		bool independent_blend_enable_ = false;
		dx12::DepthMode depth_mode_ = dx12::DepthMode::TestOnWriteOn;
		dx12::RasterizerMode rasterizer_mode_ = dx12::RasterizerMode::CullNoneSolid;
		DXGI_FORMAT dsv_format_ = DXGI_FORMAT_D24_UNORM_S8_UINT;
		bool front_counter_clockwise_ = true;
	};

	//描画パイプライン
	class GraphicsPipelineState
	{
	public:
		GraphicsPipelineState(ID3D12Device* device, const GraphicsPipelineDesc& graphics_pipeline_desc, LPCWSTR pipeline_name);
		GraphicsPipelineState(ID3D12Device8* device, const MeshShaderPipelineDesc& mesh_shader_pipeline_desc);
		GraphicsPipelineState(ID3D12Device* device, 
			D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc, 
			const wchar_t* vs_filename, const wchar_t* ps_filename);

		/**
		 * \brief このクラスが持っているパイプラインをルートシグネチャをコマンドリストにセットする
		 * \param command_list ID3D12GraphicsCommandList*
		 */
		void SetOnCommandList(ID3D12GraphicsCommandList* command_list) const
		{
			command_list->SetGraphicsRootSignature(root_signature_.Get());
			command_list->SetPipelineState(pipeline_state_.Get());
		}

		/**
		 * \brief シェーダーを取得
		 * \param shader_type Shader::Type
		 * \return 結果　取得に失敗した場合はnullptrが返る
		 */
		std::shared_ptr<Shader> GetShader(Shader::Type shader_type) const;
	private:

		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipeline_state_{};
		Microsoft::WRL::ComPtr<ID3D12RootSignature> root_signature_;
		std::unordered_map<Shader::Type, std::shared_ptr<Shader>> shader_;
	};

}

