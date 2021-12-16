#pragma once
#include "scene/Scene.h"
#include "GameObject/GameObjectManager.h"

namespace JimaXengine
{
	class TestScene : public Scene
	{
		// joycon Žü‚è‚±‚±‚©‚ç
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

		Vector3 row_accel, row_gyro;

		Vector3 position, velocity, accel, rotation;
		Vector3 scale;
		Vector4 color;

	private:
		void JoyConInitialize();
		void JoyConUpdate();

		// joycon Žü‚è‚±‚±‚Ü‚Å

	private:
		std::unique_ptr<Camera> camera;
		GameObjectManager* oManager = nullptr;
		Light* light = nullptr;

		Model* model = nullptr;
		Object3d* object = nullptr;

	public:
		TestScene();
		~TestScene();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}
