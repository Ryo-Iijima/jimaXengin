#pragma once
#include "../math/Vector3.h"
//#include "ParticleParam.h"

class GameObject;
class ParticleManager;
class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();
	void Initialize(ParticleManager* manager);

	// テンプレートジオメトリパーティクル(書き方確認、仮呼び出し用)
	void TempleteGeo(const Vector3& pos);
	// テンプレートメッシュパーティクル(書き方確認、仮呼び出し用)
	// 形状を指定したい場合のみ(重い)
	void TempleteObj(const Vector3& pos);

	// 敵に当たった時、岩が砕け散る
	void CrashBlock(const Vector3& pos);
	// 移動時、火花的な(未完成)
	void Drift(const Vector3& pos, const Vector3& vec);
	// ビルボードのよくわからんもや
	void RandomEffect(const Vector3& pos);
	// アイテムの周りに出すエフェクト
	void Moya(const Vector3& pos, const Vector3& color = Vector3::One);
	// 球状に出るエフェクト
	void SphereEffect(const Vector3& pos, const Vector3& color = Vector3::One);
	// 球状に出るエフェクト(アイテム生成前)
	void PreSphereEffect(const Vector3& pos, const Vector3& color = Vector3::One);
	// アイテム取得時のエフェクト
	void RandomCube(const Vector3& pos);
	// アイテム取得時のエフェクト
	void RandomCubeHoming(const Vector3& pos, GameObject* gmanager);
	// 追従
	void Homing(const Vector3& pos, GameObject* gmanager);
	// ドリルふっとぶ的な
	void AwayDrill(const Vector3& pos);
	// プレイヤー無敵
	void InvincibleEffect(const Vector3& pos);

private:
	ParticleManager* manager = nullptr;
};