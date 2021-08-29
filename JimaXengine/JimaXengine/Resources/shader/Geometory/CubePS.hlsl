#include "Particle.hlsli"

float4 main(GSOutput input) : SV_TARGET
{
	float4 texcolor = tex.Sample(smp, input.uv) * input.color;
	
	float3 light = normalize(float3(0, -1, 0));
	float brightness = dot(-light, input.normal.xyz);
	brightness += 0.5f;

	return float4(texcolor.rgb * brightness, texcolor.a);
}