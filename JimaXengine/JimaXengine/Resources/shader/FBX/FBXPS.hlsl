#include "FBXShader.hlsli"

Texture2D<float4> tex :register(t0);
SamplerState smp : register(s0);

float4 main(VSOutput input) : SV_TARGET
{
	//return float4(1,0,0,1);

	float4 texcolor = tex.Sample(smp,input.uv);

	//return texcolor;

	// Lambert反射
	//float3 light = normalize(lightv);
	//float diffuse = saturate(dot(-light, input.normal));
	//float brightness = diffuse + 0.3f;
	//float4 shadecolor = float4(brightness, brightness, brightness, 1.0f);
	//return shadecolor * texcolor * color;

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
	//float3 specular = pow(saturate(dot(reflect, eyedir)), shinines) * m_specular;
	float3 specular = { 0.5,0.5,0.5 };

	shadecolor.rgb = (ambient + diffuse + specular) * lightcolor;
	//shadecolor.rgb = (ambient + diffuse) * lightcolor;
	shadecolor.a = m_alpha;

	return shadecolor * texcolor * color;
}