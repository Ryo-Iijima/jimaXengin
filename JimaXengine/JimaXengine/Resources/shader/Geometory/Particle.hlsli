Texture2D<float4> tex : register(t0);	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);		// 0番スロットに設定されたサンプラー

//定数バッファ
cbuffer cbuff0 : register(b0)
{
	matrix matV;
	matrix matP;
	matrix matBillboard;
};

//頂点シェーダからジオメトリシェーダへのやり取りに使用する構造体
struct VSOutput
{
	float4 pos : POSITION;
	float scale : TEXCOORD;
	float3 rotation : ROTATION;
	float4 color : COLOR;
};

//ジオメトリシェーダからピクセルシェーダへの出力
struct GSOutput
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 normal : NORMAL;
	float4 color : COLOR;
};

//法線の計算
float4 CalNormal(float4 pos0, float4 pos1, float4 pos2)
{
	float4 vec1 = pos1 - pos0;
	float4 vec2 = pos2 - pos0;
	float4 normal = float4(normalize(cross(vec1.xyz, vec2.xyz)), 0);
	return normal;
}

//度→ラジアン
float ConvertToRadians(float degrees)
{
	return degrees * (3.1415f / 180.0f);
}

//単位行列
float4x4 Identity()
{
	float4x4 result = 
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

//X軸回転行列
float4x4 RotationX(float x)
{
	float sin_value = sin(x);
	float cos_value = cos(x);

	float4x4 result =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos_value,  sin_value,  0.0f,
		0.0f, -sin_value, cos_value,  0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

//Y軸回転行列
float4x4 RotationY(float y)
{
	float sin_value = sin(y);
	float cos_value = cos(y);

	float4x4 result =
	{
		cos_value,  0.0f, -sin_value, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		sin_value,  0.0f, cos_value,  0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

//Z軸回転行列
float4x4 RotationZ(float z)
{
	float sin_value = sin(z);
	float cos_value = cos(z);

	float4x4 result =
	{
		cos_value,  sin_value,  0.0f, 0.0f,
		-sin_value, cos_value,  0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return result;
}

//回転行列(一括用)
float4x4 RotationXYZ(float3 rot)
{
	float4x4 result = Identity();

	result = mul(RotationX(ConvertToRadians(rot.x)), result);
	result = mul(RotationY(ConvertToRadians(rot.y)), result);
	result = mul(RotationZ(ConvertToRadians(rot.z)), result);

	return result;
}