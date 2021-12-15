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

		int hp;					// �q�b�g�|�C���g
		const int Maxhp = 3;	// �ő�q�b�g�|�C���g

		std::unique_ptr<Object2d> damageSprite;		// ���H������Ƃ��ɉ�ʂ̎����Ԃ�����p
		bool damaged = false;						// �_���[�W�_�ŗp
		bool half = false;							// �����p
		const int damageTime = 20;					// �_�Ŏ���
		int damageCount = 0;						// �_�ŃJ�E���g�p
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
			std::unique_ptr<Object3d> object;	// ���f��
			Vector3 pos;						// �ʒu
			Vector3 scale;						// �X�P�[��
			Vector3 rot;						// ��]
			Vector4 color;						// �F
			Vector3 vel;						// �ړ���
			Vector3 acc;						// �����x
			AABB3D col;							// �����蔻��p
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
		void Damage();	// �_���[�W��H�炤


	// joycon ���肱������
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

	// joycon ���肱���܂�


	};
}