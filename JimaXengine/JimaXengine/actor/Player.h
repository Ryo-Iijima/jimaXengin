#pragma once
#include "../hidapi/hidapi.h"
#include "../GameObject/GameObject.h"
#include "../NumberDrawer.h"
#include "../general/Sound.h"
#include "../BatterCharacter.h"

namespace JimaXengine
{
	class Camera;
	class Object3d;
	class Object2d;
	class Sound;

	class Player : public GameObject
	{
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

			bool isHold;						// 構えているか
			bool preIsHold;						// 前のフレーム構えていたか
			float holdCount;					// 構えてからの引付時間
		};

		std::unique_ptr<Racket> leftRacket;
		std::unique_ptr<Racket> rightRacket;

	private:
		std::unique_ptr <Object3d> object;

		std::unique_ptr <BatterCharacter> character;

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

		int hitBollCount;					// ボスに球を当てた数
		int shotBollCount;					// ボスが打った数
		NumberDrawer* hitBollCountTex;
		NumberDrawer* shotBollCountTex;

		/// カメラアクション関連 ///
		// 開始位置
		Vector3 cameraStartPos;
		// 終了位置
		Vector3 cameraEndPos;
		// 移動完了フラグ
		bool moveEnd;
		// 移動ルートとかはそれぞれ作る？時間経過で方向とか変えてやる

		/// 打つタイミング関連 ///
		bool isSwing;			// 振っている最中
		bool isSwung;			// 振り切った
		bool isHitZone;			// 当たる位置にあるか
		int swingCounter;		// 振ってる間の計測タイマー
		int justHitTime;		// 当たる位置にある時間
		int justHitinterval;	// （上の時間からどれだけ続くか）
		int swingTime;			// 振り始めてから振り切るまでの時間
		int holdTime;			// 振り切ってから再び構えるまでの時間

	public:
		Player(Camera* camera);
		~Player();

		void Initialize()override;
		void Update()override;
		void Draw()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		void DrawAlphaObj();
		void Damage();	// ダメージを食らう

		// 敵に球を当てた回数を加算
		void AddHitBollCount() { hitBollCount++; }

		// setter
		void SetCamera(Camera* camera) { pCamera = camera; }

		// getter
		Camera* GetCamera();
		bool GetIsHitZone() { return isHitZone; }

	private:
		// 移動他
		void Move();
		// バットを振る
		void Swing();
	};
}