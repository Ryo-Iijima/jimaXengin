#pragma once
#include "math/Vector3.h"

namespace JimaXengine
{
	/// <summary>
	/// ポイントライト
	/// </summary>
	class PointLight
	{
	public: // サブクラス
		// 定数バッファ用構造体
		struct ConstBufferData
		{
			Vector3 lightPos;
			float pad1;
			Vector3 lightColor;
			float pad2;
			Vector3 lightAtten;
			unsigned int active;
		};

	private: // メンバ変数
		// 座標
		Vector3 lightPos = { 0.0f, 0.0f, 0.0f };
		// 色
		Vector3 lightColor = { 1.0f, 1.0f, 1.0f };
		// 距離減衰係数
		Vector3 lightAtten = { 1.0f, 1.0f, 1.0f };
		// 有効フラグ
		bool active = false;

	public: // メンバ関数
		// setter
		inline void SetLightPos(const Vector3& lightpos) { lightPos = lightpos; }
		inline void SetLightColor(const Vector3& lightcolor) { lightColor = lightcolor; }
		inline void SetLightAtten(const Vector3& lightatten) { lightAtten = lightatten; }
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const Vector3& GetLightPos() { return lightPos; }
		inline const Vector3& GetLightColor() { return lightColor; }
		inline const Vector3& GetLightAtten() { return lightAtten; }
		inline const bool IsActive() { return active; }
	};
}