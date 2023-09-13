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

    //多分ミップマップを使うよう？　おそらく 今は０固定(roughnessに0を入れているため)
    float lod = roughness * float(number_of_levels - 1);
    v = normalize(v);

	//zとxからタンジェントを求めそれを角度に変換する　その場合、範囲は
    //-pi ~ piになる　uv座標は0~1の範囲に収めたいのでいい感じになるように計算を行う
    //yの場合はサインから角度を求め(例によって-pi~pi)、真上向いたときはPI(このときのV座標は０）
    //真下を向いたときは-pI(このときのV座標は1)になるようないい感じの計算式を使う
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
    //UV座標を用いてNDC座標に変換する 背景の場合は一番奥に描画されるため
    //Z値は１
    ndc.x = (pin.texcoord_.x * 2.0) - 1.0;
    ndc.y = 1.0 - (pin.texcoord_.y * 2.0);
    ndc.z = 1;
    ndc.w = 1;

    ConstantBuffer<SceneConstant> sceneConstant = ResourceDescriptorHeap[render_resource.scene_constant_index_];

    //ビュープロジェクション行列の逆行列をかけることでワールド座標へ変換
    float4 R = mul(ndc, sceneConstant.camera_data_.inv_view_projection_);

    float3 ray_camera_to_world_position = normalize(R.xyz - sceneConstant.camera_data_.position_.xyz);

    ray_camera_to_world_position /= R.w;

    //これはなんですか？？？
    //W成分は視錐台の拡大率にあたるらしい
    //そのためw成分で割ることで-1~1の範囲に値を収められる
    //ただの座標変換では直接使うことはないがプロジェクション行列が関わったら
    //重要っぽい…
    //つまりNDC上に配置できるってこと？？？
	R /= R.w;
    const float roughness = 0;
    //return float4(1, 1, 1, 1);

    //ワールドからカメラ座標に変換
    //R = mul(float4(R.xyz, 1.0f), sceneConstant.view_projection_);
    //R = R / R.w;

	//return sample_skybox(ray_camera_to_world_position, roughness);
    return sample_skybox(R.xyz, roughness);
}