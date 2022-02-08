#include "Particle.hlsli"

#pragma warning(disable : 3557)

static const uint vnum = 4;
static const float size = 0.5f;
static const uint surfaceCount = 1;
static const uint vertexCount = 6;

static const float4 offset_array[vnum] =
{
	float4(-size,-size,0,0),
	float4(-size,+size,0,0),
	float4(+size,-size,0,0),
	float4(+size,+size,0,0),
};

static const float2 uv_array[vnum] =
{
	float2(0, 1),
	float2(0, 0),
	float2(1, 1),
	float2(1, 0),
};

[maxvertexcount(surfaceCount * vertexCount)]
void main(point VSOutput input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	GSOutput element;
	matrix mat = mul(matP, matV);

	for (uint i = 0; i < surfaceCount; i++)
	{
		for (uint f = i * 4; f < i * 4 + 3; f++)
		{
			float4 offset = offset_array[f] * input[0].scale;
			offset = mul(matBillboard, offset);
			element.svpos = input[0].pos + offset;
			element.svpos = mul(mat, element.svpos);
			element.uv = uv_array[f % 4];
			element.color = input[0].color;
			element.normal = float4(0, 0, 0, 0);

			output.Append(element);
		}
		output.RestartStrip();

		for (uint s = i * 4 + 3; s > i * 4; s--)
		{
			float4 offset = offset_array[s] * input[0].scale;
			offset = mul(matBillboard, offset);
			element.svpos = input[0].pos + offset;
			element.svpos = mul(mat, element.svpos);
			element.uv = uv_array[s % 4];
			element.color = input[0].color;
			element.normal = float4(0, 0, 0, 0);

			output.Append(element);
		}
		output.RestartStrip();
	}
}