#pragma once
#include "../GameObject/GameObject.h"
#include "../OctLeg.h"

namespace JimaXengine
{	
	class Camera;
	class Model;
	class Object3d;
	class Object2d;

	class Boss : public GameObject
	{
	private:
		std::unique_ptr<Object3d> object;

		const int legNum = 8;		// 足の数
		std::vector< std::unique_ptr<OctLeg>> legs;	// タコの足

		std::shared_ptr<GameObjectManager> pOManager;

		int hp;					// ヒットポイント
		const int Maxhp = 3;	// 最大ヒットポイント
		bool damaged = false;	// ダメージ点滅用
		const Vector4 normalColor = Vector4(1, 1, 1, 1);		// 通常時の色
		const Vector4 damageColor = Vector4(1, 0.5f, 0.5f, 1);	// ダメージ受けた時の色
		int count = 20;			// 点滅時間
		int i = 0;				// 点滅カウント用
		bool isAnger = false;	// 怒り状態
		
		std::unique_ptr<Object2d> hpSprite;		// HP表示用
		const float hpBarMaxLength = 500;		// HPバーの最大長
		float hpBarLength;						// 今のHPバーの長さ

		int actionIntervalTimer = 200;		// 次に行動するまでの時間

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
			{  0,  2.0f, 0},
			{-10, 7.0f, 0},
			{ 10, 7.0f, 0},
			{-10,  2.0f, 0},
			{ 10, 2.0f, 0}
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

		// 狙いずらしの中心
		Vector3 center;
		// ずらし半径
		float rad = 1.0f;

		const int shotInterval = 100;	// 連続発射の間隔
		int shotIntervalTimer;			// 
		int shotTime = 3;				// 発射回数
		int shotCounter;				// 発射回数カウント

		// プレイヤーの方を向く用
		Vector3 playerPos;
		Vector3 dir;
		Vector3 angle = { 0,0,0 };

		// 上下浮遊用
		Vector3 floatingOffsetPos;	// モデルを上下させる用のオフセット
		float floatingOffsetWidth;	// 上下移動の幅
		float a = 0.02f;

		// 何発発射したか
		int shotBallCount;

		// 球が打てる状態か
		bool ballStandBy[3];

		// ボールの発射地点へのオフセット
		Vector3 shotPosOffset;
		std::unique_ptr<Object3d> offsetPosObj;
		float offsetRad;
		Vector3 p, s, r;
		Vector4 c;

		float bollSpeed;		// ボールのスピード

	private:
		
		/// <summary>
		/// 移動
		/// </summary>
		void Move();

		/// <summary>
		/// 単発ショット
		/// </summary>
		void SingleShot();

		/// <summary>
		/// ３連ショット
		/// </summary>
		void RapidFire();

		/// <summary>
		///  乱射
		/// </summary>
		void EachShot();

		/// <summary>
		/// プレイヤーのほうに向く
		/// </summary>
		void SuitableForPlayer();

		/// <summary>
		/// 上下にふわふわ
		/// </summary>
		void Floating();

		/// <summary>
		/// 怒り条件チェック
		/// </summary>
		void CheckIsAnger();
		
		/// <summary>
		/// ダメージ演出
		/// </summary>
		void DamageEffect();
		
		/// <summary>
		/// 判定の位置合わせ
		/// </summary>
		void ColPosSet()override;
		
		/// <summary>
		/// 足の初期位置合わせ
		/// </summary>
		void LegPosInitialize();

	public:
		Boss(std::shared_ptr<Camera> camera, std::shared_ptr<GameObjectManager> oManager);
		~Boss();
		void Initialize()override;
		void Update()override;
		void Draw()override;

		void Dead()override;
		void Delete()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		/// <summary>
		/// ダメージを食らう
		/// </summary>
		void Damage();

		// setter

		/// <summary>
		/// ボールの待機状態のセット
		/// </summary>
		/// <param name="index">配列番号</param>
		/// <param name="isstndby">投げれる状態か</param>
		void SetBallStandBy(int index, bool isstndby) { ballStandBy[index] = isstndby; }

		// Getter

		/// <summary>
		/// 打ったボールの数の取得
		/// </summary>
		/// <returns>打ったボールの数</returns>
		int GetShotBallCount() { return shotBallCount; }

	};
}