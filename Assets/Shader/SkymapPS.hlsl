#include "FullscreenQuad.hlsli"
#include "RootSignature.hlsli"
#include "Constant.h"

struct RenderResource
{
    uint texture_index_;
    uint scene_constant_index_;
};

ConstantBuffer<RenderResource> render_resource : register(b0);


float4 sample_skybox(float3 v, float roughness)
{
    Texture2D skybox = ResourceDescriptorHeap[render_resource.texture_index_];
   // TextureCube skybox = ResourceDescriptorHeap[render_resource.texture_index_];

    const float PI = 3.14159265358979;
    uint width, height, number_of_levels;
    skybox.GetDimensions(0, width, height, number_of_levels);

    //�����~�b�v�}�b�v���g���悤�H�@�����炭 ���͂O�Œ�(roughness��0�����Ă��邽��)
    float lod = roughness * float(number_of_levels - 1);
    v = normalize(v);

	//z��x����^���W�F���g�����߂�����p�x�ɕϊ�����@���̏ꍇ�A�͈͂�
    //-pi ~ pi�ɂȂ�@uv���W��0~1�͈̔͂Ɏ��߂����̂ł��������ɂȂ�悤�Ɍv�Z���s��
    //y�̏ꍇ�̓T�C������p�x������(��ɂ����-pi~pi)�A�^��������Ƃ���PI(���̂Ƃ���V���W�͂O�j
    //�^�����������Ƃ���-pI(���̂Ƃ���V���W��1)�ɂȂ�悤�Ȃ��������̌v�Z�����g��
    float2 sample_point;
    sample_point.x = (atan2(v.z, v.x) + PI) / (PI * 2.0);
    sample_point.y = 1.0 - ((asin(v.y) + PI * 0.5) / PI);
   // ConstantBuffer<Argent:: Common::
   // SceneConstant > sceneConstant = ResourceDescriptorHeap[renderResource.sceneConstantIndex];

	float4 ret = skybox.SampleLevel(anisotropicSampler, sample_point, lod);
    //float4 ret = skybox.Sample(anisotropicSampler, v);
	//  ret = InverseGammaProcess(ret, 2.2);

    v = (v + float3(1, 1, 1)) / 2.0f;
    return float4(v, 1.0f);
    return float4(ret.xyz, 1);
}

[RootSignature(BindlessRootSignature)]
float4 main(VSOut pin) : SV_TARGET
{
    float4 ndc;
    //UV���W��p����NDC���W�ɕϊ����� �w�i�̏ꍇ�͈�ԉ��ɕ`�悳��邽��
    //Z�l�͂P
    ndc.x = (pin.texcoord_.x * 2.0) - 1.0;
    ndc.y = 1.0 - (pin.texcoord_.y * 2.0);
    ndc.z = 1;
    ndc.w = 1;

    ConstantBuffer<SceneConstant> sceneConstant = ResourceDescriptorHeap[render_resource.scene_constant_index_];

    //�r���[�v���W�F�N�V�����s��̋t�s��������邱�ƂŃ��[���h���W�֕ϊ�
    float4 R = mul(ndc, sceneConstant.camera_data_.inv_view_projection_);

    float3 ray_camera_to_world_position = normalize(R.xyz - sceneConstant.camera_data_.position_.xyz);

    ray_camera_to_world_position /= R.w;

    //����͂Ȃ�ł����H�H�H
    //W�����͎�����̊g�嗦�ɂ�����炵��
    //���̂���w�����Ŋ��邱�Ƃ�-1~1�͈̔͂ɒl�����߂���
    //�����̍��W�ϊ��ł͒��ڎg�����Ƃ͂Ȃ����v���W�F�N�V�����s�񂪊ւ������
    //�d�v���ۂ��c
    //�܂�NDC��ɔz�u�ł�����Ă��ƁH�H�H
	R /= R.w;
    const float roughness = 0;
    //return float4(1, 1, 1, 1);

    //���[���h����J�������W�ɕϊ�
    //R = mul(float4(R.xyz, 1.0f), sceneConstant.view_projection_);
    //R = R / R.w;

	//return sample_skybox(ray_camera_to_world_position, roughness);
    return sample_skybox(R.xyz, roughness);
}