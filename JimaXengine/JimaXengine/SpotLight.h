#pragma once
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "general/General.h"

namespace JimaXengine
{
	/// <summary>
	/// スポットライト
	/// </summary>
	class SpotLight
	{
	private: // エイリアス
		using XMVECTOR = DirectX::XMVECTOR;

	public: // サブクラス
		// 定数バッファ用構造体
		struct ConstBufferData
		{
			XMVECTOR lightv;
			Vector3 lightPos;
			float pad1;
			Vector3 lightColor;
			float pad2;
			Vector3 lightAtten;
			float pad3;
			Vector2 lightFactorAngleCos;
			unsigned int active;
			float pad4;
		};

	private: // メンバ変数
		// ライト方向（単位ベクトル）
		XMVECTOR lightdir = { 1,0,0,0 };
		// 座標
		Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
		// 色
		Vector3 lightColor = { 1.0f, 1.0f, 1.0f };
		// 距離減衰係数
		Vector3 lightAtten = { 1.0f, 1.0f, 1.0f };
		// ライト減衰角度
		Vector2 lightFactorAngleCos = { 0.5f,0.2f };
		// 有効フラグ
		bool active = false;

	public: // メンバ関数
		// setter
		inline void SetLightDir(const XMVECTOR& lightdir) { this->lightdir = DirectX::XMVector3Normalize(lightdir); }
		inline void SetLightPos(const Vector3& lightpos) { lightPos = lightpos; }
		inline void SetLightColor(const Vector3& lightcolor) { lightColor = lightcolor; }
		inline void SetLightAtten(const Vector3& lightatten) { lightAtten = lightatten; }
		inline void SetLightFactorAngle(const Vector2& lightfactorangle) { 
			lightFactorAngleCos.x = cosf(General::ConvertToRadians(lightfactorangle.x));
			lightFactorAngleCos.y = cosf(General::ConvertToRadians(lightfactorangle.y));
		}
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const XMVECTOR& GetLightDir() { return lightdir; }
		inline const Vector3& GetLightPos() { return lightPos; }
		inline const Vector3& GetLightColor() { return lightColor; }
		inline const Vector3& GetLightAtten() { return lightAtten; }
		inline const Vector2& GetLightFactorAngleCos() { return lightFactorAngleCos; }
		inline const bool IsActive() { return active; }
	};
}