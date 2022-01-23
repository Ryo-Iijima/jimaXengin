#pragma once

#include <DirectXMath.h>
#include "math/Vector3.h"

namespace JimaXengine
{
	/// <summary>
	/// 平行光源
	/// </summary>
	class DirectionalLight
	{
	private: // エイリアス
		using XMVECTOR = DirectX::XMVECTOR;
		using XMMATRIX = DirectX::XMMATRIX;

	public: // サブクラス

		// 定数バッファ用データ構造体
		struct ConstBufferData
		{
			XMVECTOR lightv;
			Vector3 lightcolor;
			unsigned int active;
		};

	public: // メンバ関数
		// setter
		inline void SetLightDir(const XMVECTOR& lightdir) { this->lightdir = DirectX::XMVector3Normalize(lightdir); }
		inline void SetLightColor(const Vector3& lightcolor) { this->lightcolor = lightcolor; }
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const XMVECTOR& GetLightDir() { return lightdir; }
		inline const Vector3& GetLightColor() { return lightcolor; }
		inline bool IsActive() { return active; }

	private: // メンバ変数
		// ライト方向（単位ベクトル）
		XMVECTOR lightdir = { 1,0,0,0 };
		// ライト色
		Vector3 lightcolor = { 1,1,1 };
		// 有効フラグ
		bool active = false;
	};

}