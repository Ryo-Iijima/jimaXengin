#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0); //0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

PSOutput main(VSOutput input) : SV_TARGET
{
	PSOutput output;

	output.target0 = tex.Sample(smp, input.uv) * color;
	output.target1 = float4(1 - (tex.Sample(smp, input.uv) * color).rgb, (tex.Sample(smp, input.uv) * color).a);

	//return tex.Sample(smp, input.uv) * color;
	return output;
}