#pragma once
#include "math/Vector3.h"
#include "math/Vector2.h"
#include "general/General.h"

namespace JimaXengine
{
	/// <summary>
	/// 丸影
	/// </summary>
	class CircleShadow
	{
	private: // エイリアス
		using XMVECTOR = DirectX::XMVECTOR;

	public: // サブクラス
		// 定数バッファ用構造体
		struct ConstBufferData
		{
			XMVECTOR dir;
			Vector3 casterPos;
			float distanceCasterPos;
			Vector3 atten;
			float pad1;
			Vector2 factorAngleCos;
			unsigned int active;
			float pad2;
		};

	private: // メンバ変数
		// 方向（単位ベクトル）
		XMVECTOR dir = { 1,0,0,0 };
		// キャスターとライトの距離
		float distanceCasterLight = 100.0f;
		// キャスター座標
		Vector3 casterPos = { 0.0f, 0.0f, 0.0f };
		// 距離減衰係数
		Vector3 atten = { 0.5f, 0.6f, 0.0f };
		// 減衰角度
		Vector2 factorAngleCos = { 0.2f,0.5f };
		// 有効フラグ
		bool active = false;

	public: // メンバ関数
		// setter
		inline void SetDir(const XMVECTOR& dir) { this->dir = DirectX::XMVector3Normalize(dir); }
		inline void SetCasterPos(const Vector3& casterpos) { casterPos = casterpos; }
		inline void SetDistanceCasterLight(float distancecasterlight) { distanceCasterLight = distancecasterlight; }
		inline void SetAtten(const Vector3& atten) { this->atten = atten; }
		inline void SetFactorAngle(const Vector2& factorangle) {
			factorAngleCos.x = cosf(General::ConvertToRadians(factorangle.x));
			factorAngleCos.y = cosf(General::ConvertToRadians(factorangle.y));
		}
		inline void SetActive(bool active) { this->active = active; }

		// getter
		inline const XMVECTOR& GetDir() { return dir; }
		inline const Vector3& GetCasterPos() { return casterPos; }
		inline const float GetDistanceCasterLight() { return distanceCasterLight; }
		inline const Vector3& GetAtten() { return atten; }
		inline const Vector2& GetFactorAngleCos() { return factorAngleCos; }
		inline const bool IsActive() { return active; }
	};
}