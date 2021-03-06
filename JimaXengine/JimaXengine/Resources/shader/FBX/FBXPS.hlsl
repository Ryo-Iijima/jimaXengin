#include "FBXShader.hlsli"

//#pragma warning(disable : 3078)

Texture2D<float4> tex :register(t0);
SamplerState smp : register(s0);

struct PSOutput
{
	float4 target0 : SV_TARGET0;
	float4 target1 : SV_TARGET1;
};

PSOutput main(VSOutput input)
{
	PSOutput output;

	int i;

	//return float4(1,0,0,1);

	// テクスチャマッピング
	float4 texcolor = tex.Sample(smp,input.uv);

	//return texcolor;

	// 光沢度
	const float shininess = 4.0f;
	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.svpos.xyz);

	// 環境反射光
	float3 ambient = m_ambient;

	// シェーディングによる色
	float4 shadecolor = { 0,0,0,0 };

	// 平行光源
	for (i = 0; i < DIRLIGHT_NUM; i++) {
		if (dirLights[i].active) {
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(dirLights[i].lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			shadecolor.rgb += (ambient + diffuse + specular) * dirLights[i].lightcolor;
			shadecolor.a = m_alpha;
		}
	}

	// 点光源
	for (i = 0; i < POINTLIGHT_NUM; i++) {
		if (pointLights[i].activ) {
			// ライトのベクトル
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			// ベクトルの長さ
			float d = length(lightv);
			// 正規化し、単位ベクトルにする
			lightv = normalize(lightv);
			// 距離減衰係数
			float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
			// すべて加算
			shadecolor.rgb += atten*(ambient + diffuse + specular) * pointLights[i].lightcolor;
			shadecolor.a = m_alpha;
		}
	}

	// スポットライト
	for (i = 0; i < SPOTLIGHT_NUM; i++) {
		if (spotLights[i].activ) {
			// ライトの方向ベクトル
			float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
			float d = length(lightv);
			lightv = normalize(lightv);
			// 距離減衰係数
			float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d + spotLights[i].lightatten.z * d * d));
			// 角度減衰
			float cos = dot(lightv, spotLights[i].lightv);
			// 減衰開始角度から、減衰終了角度にかけて減衰
			// 減衰開始角度の内側は１倍	減衰終了角度の外側は０倍の輝度
			float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
			// 角度減衰を乗算
			atten *= angleatten;
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// すべて加算する
			shadecolor.rgb += atten * (ambient + diffuse + specular) * spotLights[i].lightcolor;
			shadecolor.a = m_alpha;
		}
	}

	// 丸影
	for (i = 0; i < CIRCLESHADOW_NUM; i++) {
		if (circleShadows[i].activ) {
			// オブジェクトの表面からキャスターへのベクトル
			float3 casterv = circleShadows[i].casterpos - input.worldpos.xyz;
			// 投影方向での距離
			float d = dot(casterv, circleShadows[i].dir);
			// 距離減衰係数
			float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			// 距離がマイナスなら0にする
			atten *= step(0, d);
			// 仮想ライトの座標
			float3 lightpos = circleShadows[i].casterpos + circleShadows[i].dir * circleShadows[i].distancecasterlight;
			// オブジェクト表面からライトへのベクトル
			float3 lightv = normalize(lightpos - input.worldpos.xyz);
			// 角度減衰
			float cos = dot(lightv, circleShadows[i].dir);
			// 減衰開始角度から、減衰終了角度にかけて減衰
			// 減衰開始角度の内側は１倍	減衰終了角度の外側は０倍の輝度
			float angleatten = smoothstep(circleShadows[i].factoranglecos.y, circleShadows[i].factoranglecos.x, cos);
			// 角度減衰を乗算
			atten *= angleatten;

			// すべて減算
			shadecolor.rgb -= atten;
			shadecolor.a = m_alpha;
		}
	}
	output.target0 = shadecolor * texcolor * color;
	//output.target0 = float4(1, 0, 0, 1);
	output.target1 = output.target0;

	return output;
}