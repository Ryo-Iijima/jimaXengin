Texture2D<float4> tex : register(t0);	// 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);		// 0番スロットに設定されたサンプラー

// 定数バッファ
cbuffer cbuff0 : register(b0)
{
	matrix mat;		// 変換行列
	float4 color;
}

// 頂点バッファの入力
struct VSInput
{
	float4 pos	  : POSITION;
	float3 normal : NORMAL;		// 法線
	float2 uv	  : TEXCOORD;	// テクスチャ座標
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct Output
{
	float4 svpos : SV_POSITION;	// システム用の頂点座標
	float3 normal : NORMAL;		// 法線ベクトル
	float2 uv : TEXCOORD;		// uv値
};

