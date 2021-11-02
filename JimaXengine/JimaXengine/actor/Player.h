#pragma once
#include "../hidapi/hidapi.h"
#include "../GameObject/GameObject.h"

namespace JimaXengine
{
	class Camera;
	class Model;
	class Object3d;

	class Player : public GameObject
	{
	private:
		struct Racket
		{
			std::unique_ptr<Object3d> object;	// ���f��
			Vector3 pos;						// �ʒu
			Vector3 vel;						// ���x
			AABB3D col;							// �����蔻��p
		};
	private:
		Model* model = nullptr;
		Object3d* object = nullptr;
		Vector3 eye, target;

	public:
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

		// joycon ����
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
		void Move();

	};
}