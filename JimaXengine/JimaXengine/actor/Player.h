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
		//std::array<std::unique_ptr<Object2d>, 4> HP_UI;
		std::unique_ptr<Object2d> hpUi_0;
		std::unique_ptr<Object2d> hpUi_1;
		std::unique_ptr<Object2d> hpUi_2;
		std::unique_ptr<Object2d> hpUi_3;

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


	// joycon 周りここから
	private:
		Object3d* layObj = nullptr;

		hid_device_info* device = nullptr;
		hid_device* dev = nullptr;
		uint8_t buff[0x40];
		size_t size;

		struct Int3
		{
			short x;
			short y;
			short z;
		};

		Int3 accel, gyro;

	private:
		void JoyConInitialize();
		void JoyConUpdate();

	// joycon 周りここまで


	};
}