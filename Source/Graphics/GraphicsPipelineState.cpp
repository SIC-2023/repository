#include "GraphicsPipelineState.h"

#include <string>
#include <vector>

#include "d3d12shader.h"
#include "d3dx12.h"

#include "ShaderCompiler.h"
#include "../Utility/Misc.h"
#include "Dx12/Dx12Utility.h"

//TODO パイプラインマネージャができたら消すこと
#include "GraphicsEngine.h"
#include "../Core/Engine.h"

namespace argent::graphics
{
	GraphicsPipelineState::GraphicsPipelineState(ID3D12Device* device,
		const GraphicsPipelineDesc& graphics_pipeline_desc, LPCWSTR pipeline_name)
	{
		//必須シェーダーのみ作成
		shader_[Shader::Type::Vertex] = std::make_shared<Shader>(graphics_pipeline_desc.vs_filename_, Shader::Type::Vertex);

		//ルートシグネチャ
		Microsoft::WRL::ComPtr<IDxcBlob> root_signature_blob;
		HRESULT hr = shader_[Shader::Type::Vertex]->GetCompileResult()->GetOutput(DXC_OUT_ROOT_SIGNATURE, IID_PPV_ARGS(root_signature_blob.ReleaseAndGetAddressOf()), nullptr);
		_ARGENT_ASSERT_EXPR(hr);

		hr = device->CreateRootSignature(0u, root_signature_blob->GetBufferPointer(), root_signature_blob->GetBufferSize(),
			IID_PPV_ARGS(root_signature_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_pipeline_state_desc{};
		graphics_pipeline_state_desc.pRootSignature = root_signature_.Get();
		graphics_pipeline_state_desc.VS.pShaderBytecode = shader_[Shader::Type::Vertex]->GetBufferPointer();
		graphics_pipeline_state_desc.VS.BytecodeLength = shader_[Shader::Type::Vertex]->GetBufferSize();
		if(graphics_pipeline_desc.ps_filename_)
		{
			shader_[Shader::Type::Pixel] = std::make_shared<Shader>(graphics_pipeline_desc.ps_filename_, Shader::Type::Pixel);
			graphics_pipeline_state_desc.PS.pShaderBytecode = shader_[Shader::Type::Pixel]->GetBufferPointer();
			graphics_pipeline_state_desc.PS.BytecodeLength = shader_[Shader::Type::Pixel]->GetBufferSize();
		}
		if (graphics_pipeline_desc.ds_filename_)
		{
			shader_[Shader::Type::Domain] = std::make_shared<Shader>(graphics_pipeline_desc.ds_filename_, Shader::Type::Domain);
			graphics_pipeline_state_desc.DS.pShaderBytecode = shader_[Shader::Type::Domain]->GetBufferPointer();
			graphics_pipeline_state_desc.DS.BytecodeLength = shader_[Shader::Type::Domain]->GetBufferSize();
		}
		if (graphics_pipeline_desc.hs_filename_)
		{
			shader_[Shader::Type::Hull] = std::make_shared<Shader>(graphics_pipeline_desc.hs_filename_, Shader::Type::Hull);
			graphics_pipeline_state_desc.HS.pShaderBytecode = shader_[Shader::Type::Hull]->GetBufferPointer();
			graphics_pipeline_state_desc.HS.BytecodeLength = shader_[Shader::Type::Hull]->GetBufferSize();
		}
		if (graphics_pipeline_desc.gs_filename_)
		{
			shader_[Shader::Type::Geometry] = std::make_shared<Shader>(graphics_pipeline_desc.gs_filename_, Shader::Type::Geometry);
			graphics_pipeline_state_desc.GS.pShaderBytecode = shader_[Shader::Type::Geometry]->GetBufferPointer();
			graphics_pipeline_state_desc.GS.BytecodeLength = shader_[Shader::Type::Geometry]->GetBufferSize();
		}

		_D3D12_SHADER_DESC shader_desc{};
		shader_[Shader::Type::Vertex]->GetShaderReflection()->GetDesc(&shader_desc);

		//インプットレイアウトのシェーダーリフレクション
		std::vector<D3D12_INPUT_ELEMENT_DESC> input_element_desc;
		for (unsigned int i = 0; i < shader_desc.InputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC sigDesc;
			shader_[Shader::Type::Vertex]->GetShaderReflection()->GetInputParameterDesc(i, &sigDesc);

			D3D12_INPUT_ELEMENT_DESC desc{};
			desc.SemanticName = sigDesc.SemanticName;
			desc.SemanticIndex = sigDesc.SemanticIndex;
			desc.Format = dx12::GetDxgiFormat(sigDesc.ComponentType, sigDesc.Mask);
			//todo すべてのバリューが独立したバッファのため　インプットスロットを常にずらす
			if(graphics_pipeline_desc.independent_input_layout_buffer_enable_)
			{
				desc.InputSlot = i;
			}
			else
			{
				desc.InputSlot = 0u;
			}
			//desc.InputSlot = 0;
			desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0u;
			input_element_desc.emplace_back(desc);
		}

		graphics_pipeline_state_desc.BlendState = dx12::GetBlendDesc(graphics_pipeline_desc.num_render_targets_, graphics_pipeline_desc.blend_mode_, graphics_pipeline_desc.alpha_to_coverage_enable_, graphics_pipeline_desc.independent_blend_enable_);
		graphics_pipeline_state_desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		graphics_pipeline_state_desc.RasterizerState = dx12::GetRasterizerDesc(graphics_pipeline_desc.rasterizer_mode_, graphics_pipeline_desc.front_counter_clockwise_);
		graphics_pipeline_state_desc.DepthStencilState = dx12::GetDepthStencilDesc(graphics_pipeline_desc.depth_mode_);
		graphics_pipeline_state_desc.InputLayout.NumElements = static_cast<UINT>(input_element_desc.size());
		graphics_pipeline_state_desc.InputLayout.pInputElementDescs = input_element_desc.data();
		graphics_pipeline_state_desc.PrimitiveTopologyType = graphics_pipeline_desc.primitive_topology_type_;
		graphics_pipeline_state_desc.NumRenderTargets = graphics_pipeline_desc.ps_filename_ ? graphics_pipeline_desc.num_render_targets_ : 0;
		for(UINT i = 0; i < graphics_pipeline_desc.num_render_targets_; ++i)
		{
			graphics_pipeline_state_desc.RTVFormats[i] = graphics_pipeline_desc.rtv_format_[i];
		}
		graphics_pipeline_state_desc.DSVFormat = graphics_pipeline_desc.dsv_format_;
		graphics_pipeline_state_desc.SampleDesc.Count = 1u;
		graphics_pipeline_state_desc.SampleDesc.Quality = 0u;
		graphics_pipeline_state_desc.NodeMask = 1u;
		graphics_pipeline_state_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		//TODO パイプラインマネージャができたら消すこと
		auto pipeline_library = GetEngine()->GetSubsystemLocator().Get<GraphicsEngine>()->GetPipelineLibrary();
		hr = pipeline_library->LoadGraphicsPipeline(pipeline_name, &graphics_pipeline_state_desc, IID_PPV_ARGS(pipeline_state_.ReleaseAndGetAddressOf()));
		if(FAILED(hr))
		{
			hr = device->CreateGraphicsPipelineState(&graphics_pipeline_state_desc, IID_PPV_ARGS(pipeline_state_.ReleaseAndGetAddressOf()));
			_ARGENT_ASSERT_EXPR(hr);

			hr = pipeline_library->StorePipeline(pipeline_name, pipeline_state_.Get());
			_ARGENT_ASSERT_EXPR(hr);
		}
	}

	GraphicsPipelineState::GraphicsPipelineState(ID3D12Device8* device,
		const MeshShaderPipelineDesc& mesh_shader_pipeline_desc)
	{
		D3DX12_MESH_SHADER_PIPELINE_STATE_DESC mesh_shader_pipeline_state_desc{};

		//シェーダーの作成	ここでは必須のシェーダーのみ
		shader_[Shader::Type::Mesh] = std::make_shared<Shader>(mesh_shader_pipeline_desc.ms_filename_, Shader::Type::Mesh);
		shader_[Shader::Type::Pixel] = std::make_shared<Shader>(mesh_shader_pipeline_desc.ps_filename_, Shader::Type::Pixel);

		//ルートシグネチャ
		{
			//ルートシグネチャ
			Microsoft::WRL::ComPtr<IDxcBlob> root_signature_blob;
			HRESULT hr = shader_[Shader::Type::Mesh]->GetCompileResult()->GetOutput(DXC_OUT_ROOT_SIGNATURE, IID_PPV_ARGS(root_signature_blob.ReleaseAndGetAddressOf()), nullptr);
			_ARGENT_ASSERT_EXPR(hr);

			hr = device->CreateRootSignature(0u, root_signature_blob->GetBufferPointer(), root_signature_blob->GetBufferSize(),
				IID_PPV_ARGS(root_signature_.ReleaseAndGetAddressOf()));
			_ARGENT_ASSERT_EXPR(hr);

			mesh_shader_pipeline_state_desc.pRootSignature = root_signature_.Get();
		}

		mesh_shader_pipeline_state_desc.MS = { shader_.at(Shader::Type::Mesh)->GetBufferPointer(), shader_.at(Shader::Type::Mesh)->GetBufferSize() };
		mesh_shader_pipeline_state_desc.PS = { shader_.at(Shader::Type::Pixel)->GetBufferPointer(), shader_.at(Shader::Type::Pixel)->GetBufferSize() };

		//Amplification Shaderの作成
		if(mesh_shader_pipeline_desc.as_filename_)
		{
			shader_[Shader::Type::Amplification] = std::make_shared<Shader>(mesh_shader_pipeline_desc.as_filename_, Shader::Type::Amplification);
			mesh_shader_pipeline_state_desc.AS = { shader_.at(Shader::Type::Amplification)->GetBufferPointer(), shader_.at(Shader::Type::Amplification)->GetBufferSize() };
		}

		mesh_shader_pipeline_state_desc.NumRenderTargets = mesh_shader_pipeline_desc.num_render_targets_;
		for(UINT i = 0; i < mesh_shader_pipeline_desc.num_render_targets_; ++i)
		{
			mesh_shader_pipeline_state_desc.RTVFormats[i] = mesh_shader_pipeline_desc.rtv_format_[i];
		}

		mesh_shader_pipeline_state_desc.DSVFormat = mesh_shader_pipeline_desc.dsv_format_;
		mesh_shader_pipeline_state_desc.RasterizerState = dx12::GetRasterizerDesc(mesh_shader_pipeline_desc.rasterizer_mode_, mesh_shader_pipeline_desc.front_counter_clockwise_);
		mesh_shader_pipeline_state_desc.BlendState = dx12::GetBlendDesc(mesh_shader_pipeline_desc.num_render_targets_, mesh_shader_pipeline_desc.blend_mode_, mesh_shader_pipeline_desc.alpha_to_coverage_enable_, mesh_shader_pipeline_desc.independent_blend_enable_);
		mesh_shader_pipeline_state_desc.DepthStencilState = dx12::GetDepthStencilDesc(mesh_shader_pipeline_desc.depth_mode_);
		mesh_shader_pipeline_state_desc.SampleMask = UINT_MAX;
		mesh_shader_pipeline_state_desc.SampleDesc = DXGI_SAMPLE_DESC(1, 0u);

		auto pso_stream = CD3DX12_PIPELINE_MESH_STATE_STREAM(mesh_shader_pipeline_state_desc);
		D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc{};

		pipeline_state_stream_desc.pPipelineStateSubobjectStream = &pso_stream;
		pipeline_state_stream_desc.SizeInBytes = sizeof(pso_stream);

		HRESULT hr = device->CreatePipelineState(&pipeline_state_stream_desc, IID_PPV_ARGS(pipeline_state_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);

	}

	GraphicsPipelineState::GraphicsPipelineState(ID3D12Device* device,
	                                             D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_state_desc, const wchar_t* vs_filename, const wchar_t* ps_filename)
	{
		//必須シェーダーのみ作成
		shader_[Shader::Type::Vertex] = std::make_shared<Shader>(vs_filename, Shader::Type::Vertex);
		shader_[Shader::Type::Pixel] = std::make_shared<Shader>(ps_filename, Shader::Type::Pixel);

		//ルートシグネチャ
		Microsoft::WRL::ComPtr<IDxcBlob> root_signature_blob;
		HRESULT hr = shader_[Shader::Type::Vertex]->GetCompileResult()->GetOutput(DXC_OUT_ROOT_SIGNATURE, IID_PPV_ARGS(root_signature_blob.ReleaseAndGetAddressOf()), nullptr);
		_ARGENT_ASSERT_EXPR(hr);

		hr = device->CreateRootSignature(0u, root_signature_blob->GetBufferPointer(), root_signature_blob->GetBufferSize(),
			IID_PPV_ARGS(root_signature_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
		pipeline_state_desc.pRootSignature = root_signature_.Get();
		pipeline_state_desc.VS.pShaderBytecode = shader_[Shader::Type::Vertex]->GetBufferPointer();
		pipeline_state_desc.VS.BytecodeLength = shader_[Shader::Type::Vertex]->GetBufferSize();
		pipeline_state_desc.PS.pShaderBytecode = shader_[Shader::Type::Pixel]->GetBufferPointer();
		pipeline_state_desc.PS.BytecodeLength = shader_[Shader::Type::Pixel]->GetBufferSize();

		_D3D12_SHADER_DESC shader_desc{};
		shader_[Shader::Type::Vertex]->GetShaderReflection()->GetDesc(&shader_desc);

		hr = device->CreateGraphicsPipelineState(&pipeline_state_desc, IID_PPV_ARGS(pipeline_state_.ReleaseAndGetAddressOf()));
		_ARGENT_ASSERT_EXPR(hr);
	}

	std::shared_ptr<Shader> GraphicsPipelineState::GetShader(Shader::Type shader_type) const
	{
		std::shared_ptr<Shader> ret{};
		if (!shader_.contains(shader_type)) _ASSERT_EXPR(FALSE, L"シェーダーに対して無効なアクセスがありました");

		ret = shader_.at(shader_type);
		return ret;
	}
}
