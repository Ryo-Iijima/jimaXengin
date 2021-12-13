#include "FBXShader.hlsli"

Texture2D<float4> tex :register(t0);
SamplerState smp : register(s0);

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

PSOutput main(VSOutput input) : SV_TARGET
{
	PSOutput output;

	//return float4(1,0,0,1);

	float4 texcolor = tex.Sample(smp,input.uv);

	//return texcolor;

	// シェーディングによる色
	float4 shadecolor;
	// 光沢度
	const float shinines = 4.0f;
	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.svpos.xyz);
	// ライトに向かうベクトルの法線の内積
	float3 dotlightnormal = dot(lightv, input.normal);
	// 反射光ベクトル
	float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
	// 環境反射光
	float3 ambient = m_ambient;
	// 拡散反射光
	float3 diffuse = dotlightnormal * m_diffuse;
	//// 鏡面反射光
	float3 specular = pow(saturate(dot(reflect, eyedir)), shinines) * m_specular;

	shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
	shadecolor.a = m_alpha;

	output.target0 = shadecolor * texcolor * color;
	output.target1 = output.target0;

	return output;
}