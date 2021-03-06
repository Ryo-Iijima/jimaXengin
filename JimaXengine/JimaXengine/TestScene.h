#pragma once
#include "scene/Scene.h"
#include "GameObject/GameObjectManager.h"
#include "hidapi/hidapi.h"
#include <vector>

namespace JimaXengine
{
	class TestScene : public Scene
	{
		///// joycon 周りここから //////
	private: // クラス内変数
		hid_device_info* device = nullptr;
		hid_device* dev = nullptr;
		uint8_t buff[0x40] = {};
		size_t size = 0;

		struct Int3
		{
			short x;
			short y;
			short z;
		};

		// 一定以下の変化を無視する用
		float deadZone = 0.01f;
		// クランプ用、変化の最大値,最小値
		Vector3 maxValue = { 0,0,0 };
		Vector3 minValue = { 0,0,0 };

		float speed = 1.0f;

		Vector3 row_accel, row_gyro;
		Vector3 accel, gyro;
		Vector3 prev_accel, prev_gyro;

		Vector3 diff_accel, prev_diff_accel;
		Vector3 diff_gyro;
		Vector3 add_gyro;

		Vector3 position, velocity, rotation;
		Vector3 scale;
		Vector4 color;

		std::vector<Vector3> arr;

		// ジャイロの値を絞る用、感度調整
		float squeeze = 1.0f;

	private:// クラス内関数
		void JoyConInitialize();
		void JoyConUpdate();

		////// joycon 周りここまで //////

	private:
		// カメラ
		std::shared_ptr<Camera> camera;

		// goマネージャー
		std::unique_ptr<GameObjectManager> oManager;
		
		// ライトグループ
		LightGroup* light = nullptr;

		// 箱のモデル
		std::unique_ptr<Object3d> object;

		// 格納するための配列
		std::vector<std::string> sensorData;

	public:
		TestScene();
		~TestScene();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}
