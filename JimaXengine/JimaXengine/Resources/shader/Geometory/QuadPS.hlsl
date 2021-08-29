#include "Particle.hlsli"

float4 main(GSOutput input) : SV_TARGET
{
	return tex.Sample(smp, input.uv) * input.color;
}