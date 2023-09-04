#pragma once

#include <intsafe.h>

#include "Shader.h"
#include "../../External/DirectXShaderCompiler/Inc/dxcapi.h"

namespace argent::graphics
{
	HRESULT CompileShader(const wchar_t* filename, Shader::Type shader_type, IDxcResult** result);
	HRESULT CreateShaderReflection(const DxcBuffer& dxc_buffer, ID3D12ShaderReflection** pp_shader_reflection);
}

