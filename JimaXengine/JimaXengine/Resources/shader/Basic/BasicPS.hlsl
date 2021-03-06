#include "BasicShader.hlsli"

float4 PSmain(Output input) : SV_TARGET
{
	float4 texColor = float4(tex.Sample(smp,input.uv));

	float3 light = normalize(float3(1,-1,1));	// ライト
	float diffuse = saturate(dot(-light, input.normal));	// diffuseを[0,1]にClampする
	float brightness = diffuse + 0.3f;	// 真っ黒にならないようにする

	//return float4(input.normal,1);
	return float4(texColor.rgb * brightness, texColor.a) * color;
	//return color;
}