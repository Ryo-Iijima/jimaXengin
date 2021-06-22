#include "BasicShader.hlsli"

float4 PSmain(Output input) : SV_TARGET
{
	return float4(input.uv, 1.0f, 1.0f);
}