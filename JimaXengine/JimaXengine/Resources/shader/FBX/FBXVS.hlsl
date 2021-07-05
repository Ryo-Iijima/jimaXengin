#include "FBXShader.hlsli"

VSOutput main(VSInput input)
{
	VSOutput output;

	// 法線にワールド行列を適用
	float4 wnormal = normalize(mul(world, float4(input.normal, 0)));
	// 行列による座標変換
	output.svpos = mul(mul(viewproj, world), input.pos);

	output.normal = wnormal.xyz;
	output.uv = input.uv;

	return output;
}