#include "ShaderCompiler.h"

#include <wrl.h>

#include <filesystem>
#include <vector>

#include "../../External/DirectXShaderCompiler/Inc/d3d12shader.h"

#include "../Utility/Misc.h"

namespace argent::graphics
{
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxc_compiler{};
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> dxc_include_handler{};
	Microsoft::WRL::ComPtr<IDxcUtils> dxc_utils{};

	HRESULT CompileShader(const wchar_t* filename, Shader::Type shader_type, IDxcResult** result)
	{
		HRESULT hr{ S_OK };
		if(!dxc_compiler.Get())
		{
			hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(dxc_utils.ReleaseAndGetAddressOf()));
			_ARGENT_ASSERT_EXPR(hr);
			hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(dxc_compiler.ReleaseAndGetAddressOf()));
			_ARGENT_ASSERT_EXPR(hr);

			hr = dxc_utils->CreateDefaultIncludeHandler(dxc_include_handler.ReleaseAndGetAddressOf());
			_ARGENT_ASSERT_EXPR(hr);
		}

		const LPCWSTR entry_point = L"main";
		LPCWSTR target_profile{};

		switch (shader_type)
		{
		case Shader::Type::Vertex:
			target_profile = L"vs_6_6";
			break;
		case Shader::Type::Pixel:
			target_profile = L"ps_6_6";
			break;
		case Shader::Type::Compute:
			target_profile = L"cs_6_6";
			break;
		case Shader::Type::Geometry:
			target_profile = L"gs_6_6";
			break;
		case Shader::Type::Domain:
			target_profile = L"ds_6_6";
			break;
		case Shader::Type::Hull:
			target_profile = L"hs_6_6";
			break;
		case Shader::Type::Mesh:
			target_profile = L"ms_6_6";
			break;
		case Shader::Type::Amplification:
			target_profile = L"as_6_6";
			break;
		default: ;
		}

		std::vector<LPCWSTR> compilation_arguments
		{
			L"-E",
			entry_point,
			L"-T",
			target_profile,
			DXC_ARG_PACK_MATRIX_ROW_MAJOR,
			DXC_ARG_WARNINGS_ARE_ERRORS,
			DXC_ARG_ALL_RESOURCES_BOUND,
			DXC_ARG_DEBUG,
			L"-I",				//インクルードディレクトリの指定
			L"./Assets/Shader",
		};

		//シェーダーファイルのロード
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> source_blob{};
		hr = dxc_utils->LoadFile(filename, nullptr, source_blob.ReleaseAndGetAddressOf());
		_ARGENT_ASSERT_EXPR(hr);

		DxcBuffer source_buffer
		{
			.Ptr = source_blob->GetBufferPointer(),
			.Size = source_blob->GetBufferSize(),
			.Encoding = 0u,
		};

		//コンパイル
		hr = dxc_compiler->Compile(&source_buffer,
			compilation_arguments.data(),
			static_cast<uint32_t>(compilation_arguments.size()),
			dxc_include_handler.Get(), IID_PPV_ARGS(result));
		_ARGENT_ASSERT_EXPR(hr);

		Microsoft::WRL::ComPtr<IDxcBlobUtf8> errors{};
		hr = (*result)->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
		_ASSERT_EXPR(SUCCEEDED(hr), HrTrace(hr));
		if (errors.Get() && errors->GetStringLength() > 0)
		{
			std::filesystem::path path2{ errors->GetStringPointer() };
			_ASSERT_EXPR(false, path2.wstring().c_str());
		}
		return hr;
	}

	HRESULT CreateShaderReflection(const DxcBuffer& dxc_buffer, ID3D12ShaderReflection** pp_shader_reflection)
	{
		if (!dxc_utils.Get()) _ASSERT_EXPR(FALSE, "Null");

		HRESULT hr = dxc_utils->CreateReflection(&dxc_buffer, IID_PPV_ARGS(pp_shader_reflection));
		_ARGENT_ASSERT_EXPR(hr);

		return hr;
	}
}
