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
			std::unique_ptr<Object3d> object;	// ���f��
			Vector3 pos;						// �ʒu
			Vector3 scale;						// �X�P�[��
			Vector3 rot;						// ��]
			Vector4 color;						// �F
			Vector3 vel;						// �ړ���
			Vector3 acc;						// �����x
			AABB3D col;							// �����蔻��p

			bool isHold;						// �\���Ă��邩
			bool preIsHold;						// �O�̃t���[���\���Ă�����
			float holdCount;					// �\���Ă���̈��t����
		};

		std::unique_ptr<Racket> leftRacket;
		std::unique_ptr<Racket> rightRacket;

	private:
		std::unique_ptr <Object3d> object;

		std::unique_ptr <BatterCharacter> character;

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
		std::unique_ptr<Object2d> hpUi_0;
		std::unique_ptr<Object2d> hpUi_1;
		std::unique_ptr<Object2d> hpUi_2;
		std::unique_ptr<Object2d> hpUi_3;

		int hitBollCount;					// �{�X�ɋ��𓖂Ă���
		int shotBollCount;					// �{�X���ł�����
		NumberDrawer* hitBollCountTex;
		NumberDrawer* shotBollCountTex;

		/// �J�����A�N�V�����֘A ///
		// �J�n�ʒu
		Vector3 cameraStartPos;
		// �I���ʒu
		Vector3 cameraEndPos;
		// �ړ������t���O
		bool moveEnd;
		// �ړ����[�g�Ƃ��͂��ꂼ����H���Ԍo�߂ŕ����Ƃ��ς��Ă��

		/// �ł^�C�~���O�֘A ///
		bool isSwing;			// �U���Ă���Œ�
		bool isSwung;			// �U��؂���
		bool isHitZone;			// ������ʒu�ɂ��邩
		int swingCounter;		// �U���Ă�Ԃ̌v���^�C�}�[
		int justHitTime;		// ������ʒu�ɂ��鎞��
		int justHitinterval;	// �i��̎��Ԃ���ǂꂾ���������j
		int swingTime;			// �U��n�߂Ă���U��؂�܂ł̎���
		int holdTime;			// �U��؂��Ă���Ăэ\����܂ł̎���

	public:
		Player(Camera* camera);
		~Player();

		void Initialize()override;
		void Update()override;
		void Draw()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		void DrawAlphaObj();
		void Damage();	// �_���[�W��H�炤

		// �G�ɋ��𓖂Ă��񐔂����Z
		void AddHitBollCount() { hitBollCount++; }

		// setter
		void SetCamera(Camera* camera) { pCamera = camera; }

		// getter
		Camera* GetCamera();
		bool GetIsHitZone() { return isHitZone; }

	private:
		// �ړ���
		void Move();
		// �o�b�g��U��
		void Swing();
	};
}