#pragma once
#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"

//パーティクルのパラメータ構造体
namespace JimaXengine
{
	struct ParticleParam
	{
		//位置、速度
		Vector3 pos = Vector3::Zero;
		Vector3 vel = Vector3::Zero;
		Vector3 acc = Vector3::Zero;
		float speed = 0.0f;
		//大きさ
		Vector3 scl = Vector3::One;
		Vector3 add_scl = Vector3::Zero;
		//回転
		Vector3 rot = Vector3::Zero;
		Vector3 add_rot = Vector3::Zero;
		//体力
		int life = 60;
		//色、透明度
		Vector3 color = Vector3::One;
		float alpha = 1.0f;
		float add_alpha = 0.0f;
	};
}