#pragma once

#include <wrl.h>

#include "../../External/DirectXShaderCompiler/Inc/dxcapi.h"
#include "../../External/DirectXShaderCompiler/Inc/d3d12shader.h"

namespace argent::graphics
{
	class Shader
	{
	public:
		enum class Type { Vertex, Pixel, Geometry, Hull, Domain, Compute, Mesh, Amplification, Count };

		Shader(const wchar_t* filename, Type type);

		void* GetBufferPointer() const { return dxc_blob_->GetBufferPointer(); }
		SIZE_T GetBufferSize() const { return dxc_blob_->GetBufferSize(); }
		Type GetType() const { return type_; }
		IDxcResult* GetCompileResult() const { return dxc_result_.Get(); }
		ID3D12ShaderReflection* GetShaderReflection() const { return shader_reflection_.Get(); }
	private:
		Type type_;
		Microsoft::WRL::ComPtr<IDxcBlob> dxc_blob_;
		Microsoft::WRL::ComPtr<IDxcResult> dxc_result_;
		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shader_reflection_;
	};
}

