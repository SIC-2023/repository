#include "Shader.h"

#include "ShaderCompiler.h"
#include "../Utility/Misc.h"

namespace argent::graphics
{
	Shader::Shader(const wchar_t* filename, Type type):
		type_(type)
	{
		HRESULT hr = CompileShader(filename, type_, dxc_result_.ReleaseAndGetAddressOf());
		_ARGENT_ASSERT_EXPR(hr);

		hr = dxc_result_->GetResult(dxc_blob_.ReleaseAndGetAddressOf());
		_ARGENT_ASSERT_EXPR(hr);

		Microsoft::WRL::ComPtr<IDxcBlob> reflection_blob{};
		hr = dxc_result_->GetOutput(DXC_OUT_REFLECTION,
			IID_PPV_ARGS(reflection_blob.ReleaseAndGetAddressOf()), nullptr);
		_ARGENT_ASSERT_EXPR(hr);

		DxcBuffer dxc_buffer{};
		dxc_buffer.Ptr = reflection_blob->GetBufferPointer();
		dxc_buffer.Size = reflection_blob->GetBufferSize();
		dxc_buffer.Encoding = 0u;

		hr = CreateShaderReflection(dxc_buffer, shader_reflection_.ReleaseAndGetAddressOf());
		_ARGENT_ASSERT_EXPR(hr);
	}
}
