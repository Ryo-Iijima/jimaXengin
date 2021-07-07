#include "BasicShader.hlsli"

Output VSmain(VSInput input)
{
	Output output;
	output.svpos = mul(mat, input.pos);
	output.normal = input.normal;
	output.uv = input.uv;

	return output;
}