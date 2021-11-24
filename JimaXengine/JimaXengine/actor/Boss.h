#pragma once
#include "../GameObject/GameObject.h"

namespace JimaXengine
{	
	class Camera;
	class Model;
	class Object3d;
	class Object2d;

	class Boss : public GameObject
	{
	private:
		Model* model;
		std::unique_ptr<Object3d> object;

		GameObjectManager* pOManager = nullptr;

		int hp;					// ヒットポイント
		const int Maxhp = 3;	// 最大ヒットポイント
		bool damaged = false;	// ダメージ点滅用
		int count = 20;			// 点滅時間
		int i = 0;				// 点滅カウント用
		
		std::unique_ptr<Object2d> hpSprite;		// HP表示用
		const float hpBarMaxLength = 500;		// HPバーの最大長
		float hpBarLength;						// 今のHPバーの長さ

		int actionIntervalTimer=200;		// 次に行動するまでの時間

		// 状態パターン
		enum class State
		{
			WAIT,		// 何もしない
			MOVE,		// 移動
			ATTACK,		// 攻撃
		};
		State state;
	
		// 移動位置
		Vector3 waitPos[5] =
		{
			{  0,  6, 0},
			{-15, 14, 0},
			{ 15, 14, 0},
			{-13,  6, 0},
			{ 13,  6, 0}
		};
		int random;
		Vector3 nextPos;				// 移動先
		Vector3 toDestinationVelocity;	// 移動先までのベクトル
		Vector3 v;						// 移動先への到着判定用
		bool moveUnDuplicate[5];		// 移動先の重複回避用

		// 攻撃種類
		enum class AttackType
		{
			SINGLESHOT,	// 単発
			RAPIDFIRE,	// 連射
			EACHSHOT,	// 拡散
		};
		AttackType attackType;
		bool attackchoseed;			// 攻撃抽選完了フラグ
		bool attacked;				// 攻撃完了フラグ
		bool attackUnDuplicate[3];	// 攻撃の重複回避用

		// プレイヤーの方を向く用
		Vector3 playerPos;
		Vector3 dir;
		Vector3 angle = { 0,0,0 };

		// 上下浮遊用
		Vector3 floatingOffsetPos;	// モデルを上下させる用のオフセット
		float floatingOffsetWidth;	// 上下移動の幅
		float a = 0.02f;

	private:
		void Move();
		void SingleShot();
		void RapidFire();
		void EachShot();
		// プレイヤーのほうに向く
		void SuitableForPlayer();
		// 上下にふわふわ
		void Floating();

	public:
		Boss(Camera* camera, GameObjectManager* oManager);
		~Boss();
		void Initialize()override;
		void Update()override;
		void Draw()override;

		void Dead()override;
		void Delete()override;
		GameObject::TYPE GetType()override;
		GameObject::RENDER_TYPE GetRenderType()override;
		void DrawImGui()override;

		void Damage();	// ダメージを食らう
	};
}