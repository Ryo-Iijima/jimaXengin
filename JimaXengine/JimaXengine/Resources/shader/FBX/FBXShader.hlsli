cbuffer cbuff0 : register(b0)
{
	float4 color;		// 色
	matrix viewproj;	// ビュープロジェクション行列
	matrix world;		// ワールド行列
	float3 cameraPos;	// カメラ座標
};

cbuffer cbuff2 : register(b2)
{
	float3 lightv;		// ライトへの方向の単位ベクトル
	float3 lightcolor;	// ライトの色
}

cbuffer cbuff4 : register(b4)
{
	float3 m_ambient;	// アンビエント係数
	float3 m_diffuse;	// ディフューズ係数
	float3 m_specular;	// スペキュラー係数
	float m_alpha;	// アルファ
}

// ボーンの最大数
static const int MAX_BONES = 32;
cbuffer skinning:register(b3)
{
	matrix matSkinning[MAX_BONES];
}

// 頂点バッファの入力
struct VSInput
{
	float4 pos	  : POSITION;
	float3 normal : NORMAL;				// 法線
	float2 uv	  : TEXCOORD;			// テクスチャ座標
	uint4 boneIndices  : BONEINDICES;	// ボーンの番号
	float4 boneWeight : BONEWEIGHTS;	// ボーンのスキンウェイト
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos  : SV_POSITION;	// システム用の頂点座標
	float3 normal : NORMAL;			// 法線ベクトル
	float2 uv	  : TEXCOORD;		// uv値
};

