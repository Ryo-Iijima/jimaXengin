#include "BasicShader.hlsli"

float4 PSmain(Output input) : SV_TARGET
{
	float4 texColor = float4(tex.Sample(smp,input.uv));

	float3 light = normalize(float3(1,-1,1));	// ƒ‰ƒCƒg
	float diffuse = saturate(dot(-light, input.normal));	// diffuse‚ğ[0,1]‚ÉClamp‚·‚é
	float brightness = diffuse + 0.3f;	// ^‚Á•‚É‚È‚ç‚È‚¢‚æ‚¤‚É‚·‚é

	//return float4(input.normal,1);
	return float4(texColor.rgb * brightness, texColor.a) * color;
	//return color;
}