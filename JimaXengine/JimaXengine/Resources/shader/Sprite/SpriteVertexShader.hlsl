#include "SpriteShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output;
	output.svpos = mul(matWP, pos);
	output.uv = uv;
	return output;
}