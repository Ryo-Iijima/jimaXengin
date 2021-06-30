#include "BasicShader.hlsli"

float4 PSmain(Output input) : SV_TARGET
{
	return float4(input.normal,1);
}