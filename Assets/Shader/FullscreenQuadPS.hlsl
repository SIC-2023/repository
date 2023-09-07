#include "FullscreenQuad.hlsli"
#include "RootSignature.hlsli"

[RootSignature(BindlessRootSignature)]
float4 main(VSOut pin) : SV_TARGET
{
	//TODO sampling texture
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}