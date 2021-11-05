#pragma once
#include "../GameObject/GameObject.h"

namespace JimaXengine
{	
	class Camera;
	class Model;
	class Object3d;

	class Boss : public GameObject
	{
	private:
		Model* model;
		std::unique_ptr<Object3d> object;

		GameObjectManager* pOManager = nullptr;

		int hp;					// ヒットポイント
		bool damaged = false;	// ダメージ点滅用
		int count = 20;			// 点滅時間
		int i = 0;				// 点滅カウント用

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
		Vector3 waitPos[6] =
		{
			{  0,  0, 50},
			{  0,  0, 50},
			{-35, 15, 50},
			{ 35, 15, 50},
			{-35,-10, 50},
			{ 35,-10, 50}
		};
		int random;
		Vector3 nextPos;
		Vector3 toDestinationVelocity;
		Vector3 v;

		// 攻撃種類
		enum class AttackType
		{
			SINGLESHOT,	// 単発
			RAPIDFIRE,	// 連射
			EACHSHOT,	// 拡散
		};
		AttackType attackType;
		bool attackchoseed;	// 攻撃抽選フラグ
		bool attacked;		// 攻撃完了フラグ
		
	private:
		void Move();
		void SingleShot();
		void RapidFire();
		void EachShot();

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