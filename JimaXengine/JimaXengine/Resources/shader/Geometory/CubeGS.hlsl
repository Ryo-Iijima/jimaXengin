#include "Particle.hlsli"

static const uint vnum = 24;
static const float size = 0.5f;
static const uint surfaceCount = 6;
static const uint vertexCount = 6;

static const float4 offset_array[vnum] =
{
	//前
	float4(-size,-size,-size,0),
	float4(-size,+size,-size,0),
	float4(+size,-size,-size,0),
	float4(+size,+size,-size,0),
	//後
	float4(+size,-size,+size,0),
	float4(+size,+size,+size,0),
	float4(-size,-size,+size,0),
	float4(-size,+size,+size,0),
	//右
	float4(+size,-size,-size,0),
	float4(+size,+size,-size,0),
	float4(+size,-size,+size,0),
	float4(+size,+size,+size,0),
	//左
	float4(-size,-size,+size,0),
	float4(-size,+size,+size,0),
	float4(-size,-size,-size,0),
	float4(-size,+size,-size,0),
	//上
	float4(-size,+size,-size,0),
	float4(-size,+size,+size,0),
	float4(+size,+size,-size,0),
	float4(+size,+size,+size,0),
	//下
	float4(-size,-size,+size,0),
	float4(-size,-size,-size,0),
	float4(+size,-size,+size,0),
	float4(+size,-size,-size,0),
};

static const float2 uv_array[4] =
{
	float2(0, 1),
	float2(0, 0),
	float2(1, 1),
	float2(1, 0),
};

[maxvertexcount(surfaceCount * vertexCount)]
void main(point VSOutput input[1] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
	matrix matRotation = RotationXYZ(input[0].rotation);
	matrix matVP = mul(matP, matV);

	//6面分
	for (uint i = 0; i < surfaceCount; i++)
	{
		//三角形一つ目
		GSOutput firstElement[3] =
		{
			{float4(0,0,0,0),float2(0,0),float4(0,0,0,0),float4(0,0,0,0)},
			{float4(0,0,0,0),float2(0,0),float4(0,0,0,0),float4(0,0,0,0)},
			{float4(0,0,0,0),float2(0,0),float4(0,0,0,0),float4(0,0,0,0)},
		};
		int firstTriIndex = 0;
		for (uint f = i * 4; f < i * 4 + 3; f++)
		{

			float4 offset = offset_array[f] * input[0].scale;
			offset = mul(matRotation, offset);
			firstElement[firstTriIndex].svpos = input[0].pos + offset;
			firstElement[firstTriIndex].svpos = mul(matVP, firstElement[firstTriIndex].svpos);
			firstElement[firstTriIndex].uv = uv_array[f % 4];
			firstElement[firstTriIndex].color = input[0].color;

			firstTriIndex++;
		}
		//法線の計算
		{
			float4 normal = CalNormal(firstElement[0].svpos, firstElement[1].svpos, firstElement[2].svpos);
			firstElement[0].normal = normal;
			firstElement[1].normal = normal;
			firstElement[2].normal = normal;
		}
		//追加
		output.Append(firstElement[0]);
		output.Append(firstElement[1]);
		output.Append(firstElement[2]);
		output.RestartStrip();

		//三角形二つ目
		GSOutput secondElement[3] = 
		{
			{float4(0,0,0,0),float2(0,0),float4(0,0,0,0),float4(0,0,0,0)},
			{float4(0,0,0,0),float2(0,0),float4(0,0,0,0),float4(0,0,0,0)},
			{float4(0,0,0,0),float2(0,0),float4(0,0,0,0),float4(0,0,0,0)},
		};
		int secondTriIndex = 0;
		for (uint s = i * 4 + 3; s > i * 4; s--)
		{
			float4 offset = offset_array[s] * input[0].scale;
			offset = mul(matRotation, offset);
			secondElement[secondTriIndex].svpos = input[0].pos + offset;
			secondElement[secondTriIndex].svpos = mul(matVP, secondElement[secondTriIndex].svpos);
			secondElement[secondTriIndex].uv = uv_array[i % 4];
			secondElement[secondTriIndex].color = input[0].color;

			secondTriIndex++;
		}
		{
			//法線の計算
			float4 normal = CalNormal(secondElement[0].svpos, secondElement[1].svpos, secondElement[2].svpos);
			secondElement[0].normal = normal;
			secondElement[1].normal = normal;
			secondElement[2].normal = normal;
		}
		//追加
		output.Append(secondElement[0]);
		output.Append(secondElement[1]);
		output.Append(secondElement[2]);
		output.RestartStrip();
	}
}