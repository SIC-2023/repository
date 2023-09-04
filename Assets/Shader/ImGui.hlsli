#ifndef _IMGUI_HLSLI
#define _IMGUI_HLSLI

#ifdef __cplusplus

#include <DirectXMath.h>

#define float2  DirectX::XMFLOAT2
#define float4  DirectX::XMFLOAT4
#define float4x4 DirectX::XMFLOAT4X4

#define uint uint32_t
#else
#pragma pack_matrix(row_major)

struct VS_IN
{
    float2 position_ : POSITION;
    float4 color_ : COLOR;
    float2 texcoord_ : TEXCOORD;
};

struct VS_OUT
{
    float4 position_ : SV_POSITION;
    float4 color_ : COLOR;
    float2 texcoord_ : TEXCOORD;
};

#endif


struct RenderResource
{
    float4x4 view_projection_matrix_;
    uint texture_index_;
};

#ifndef __cplusplus
ConstantBuffer<RenderResource> render_resource : register(b0);
#endif

#endif //_IMGUI_HLSLI