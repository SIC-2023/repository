#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#ifdef __cplusplus
#include <DirectXMath.h>

#define float2	DirectX::XMFLOAT2
#define float3	DirectX::XMFLOAT3
#define float4	DirectX::XMFLOAT4
#define float4x4	DirectX::XMFLOAT4X4

#define uint	uint32_t

#else

#pragma pack_matrix(row_major)

#endif

#define _LANE_COUNT_IN_WAVE		32
#define _MAX_BONE_NUM_			256
struct SceneConstant
{
	float4x4 view_projection_;
	float4 camera_position_;
	float4 light_direction_;
};

struct ObjectConstant
{
	float4x4 world_;
};

struct MeshConstant
{
	float4x4 local_transform_;
};

struct SkeletalMeshConstant
{
	float4x4 local_transform_;
	float4x4 bone_transform_[_MAX_BONE_NUM_];
};

struct Frustum
{
	float4 frustum_panels_[6];
	float4 eye_position_;
};

//TODO ï ÇÃèÍèäÇ…
struct PBRMaterial
{
	uint base_color_index_;
	uint normal_tex_index_;
	uint metallic_tex_index_;
	uint roughness_tex_index_;
	float metallic_factor_;
	float roughness_factor_;
};

#endif //_CONSTANT_H_