#pragma once
#include "../hidapi/hidapi.h"
#include "../GameObject/GameObject.h"

namespace JimaXengine
{
	class Camera;
	class Model;
	class Object3d;
	class Object2d;

	class Player : public GameObject
	{
	private:
		Model* model = nullptr;
		Object3d* object = nullptr;

		Vector3 eye, target;

		Vector3 racketScale;

		int hp;					// ヒットポイント
		const int Maxhp = 3;	// 最大ヒットポイント

		std::unique_ptr<Object2d> damageSprite;		// 球食らったときに画面の周りを赤くする用
		bool damaged = false;						// ダメージ点滅用
		bool half = false;							// 往復用
		const int damageTime = 20;					// 点滅時間
		int damageCount = 0;						// 点滅カウント用
		int increase = 1;

		// playerUI
		std::unique_ptr<Object2d> hpUi_0;
		std::unique_ptr<Object2d> hpUi_1;
		std::unique_ptr<Object2d> hpUi_2;
		std::unique_ptr<Object2d> hpUi_3;

		///////////////////////////
		//// 数字表示関連　↓
		///////////////////////////
		int hitBollCount;									// ボスに球を当てた数

		static const int digit = 2;							// 桁
		// ボスが打った数
		std::unique_ptr<Object2d> shotBollCountTex[digit];	// 桁数分のスプライト管理
		int shotBollCountEachNum[digit];					// 各桁の数値
		// ボスに当てた数
		std::unique_ptr<Object2d> hitBollCountTex[digit];	// 桁数分のスプライト管理
		int hitBollCountEachNum[digit];						// 各桁の数値

		///////////////////////////
		//// 数字表示関連　↑
		///////////////////////////


	public:
		struct Racket
		{
			std::unique_ptr<Object3d> object;	// モデル
			Vector3 pos;						// 位置
			Vector3 scale;						// スケール
			Vector3 rot;						// 回転
			Vector4 color;						// 色
			Vector3 vel;						// 移動量
			Vector3 acc;						// 加速度
			AABB3D col;							// 当たり判定用
		};

		std::unique_ptr<Racket> leftRacket;
		std::unique_ptr<Racket> rightRacket;

	public:
		Player(Camera* camera);
		~Player();

		void Initialize()override;
		void Update()override;
		void Draw()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		void SetCamera(Camera* camera) { pCamera = camera; }
		Camera* GetCamera();
		void DrawAlphaObj();
		void Move();
		void Damage();	// ダメージを食らう

		// 敵に球を当てた回数を加算
		void AddHitBollCount() { hitBollCount++; }

	};
}