#pragma once
#include "scene/Scene.h"
#include "GameObject/GameObjectManager.h"
#include "hidapi/hidapi.h"
#include <vector>

namespace JimaXengine
{
	class TestScene : public Scene
	{
		// joycon ���肱������
	private:
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

		// ���ȉ��̕ω��𖳎�����p
		float deadZone = 0.01f;
		// �N�����v�p�A�ω��̍ő�l,�ŏ��l
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

	private:
		void JoyConInitialize();
		void JoyConUpdate();

		// joycon ���肱���܂�

	private:
		std::unique_ptr<Camera> camera;
		GameObjectManager* oManager = nullptr;
		Light* light = nullptr;

		Model* model = nullptr;
		Object3d* object = nullptr;

		// �i�[���邽�߂̔z��
		std::vector<std::string> sensorData;

	public:
		TestScene();
		~TestScene();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}
