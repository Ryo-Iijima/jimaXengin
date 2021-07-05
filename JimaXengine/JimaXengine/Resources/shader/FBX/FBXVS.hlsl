#include "FBXShader.hlsli"

VSOutput main(VSInput input)
{
	VSOutput output;

	// �@���Ƀ��[���h�s���K�p
	float4 wnormal = normalize(mul(world, float4(input.normal, 0)));
	// �s��ɂ����W�ϊ�
	output.svpos = mul(mul(viewproj, world), input.pos);

	output.normal = wnormal.xyz;
	output.uv = input.uv;

	return output;
}