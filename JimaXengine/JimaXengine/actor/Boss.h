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
		const int legNum = 8;		// ���̐�
		std::vector< std::unique_ptr<OctLeg>> legs;	// �^�R�̑�

		GameObjectManager* pOManager = nullptr;

		int hp;					// �q�b�g�|�C���g
		const int Maxhp = 3;	// �ő�q�b�g�|�C���g
		bool damaged = false;	// �_���[�W�_�ŗp
		const Vector4 normalColor = Vector4(1, 1, 1, 1);		// �ʏ펞�̐F
		const Vector4 damageColor = Vector4(1, 0.5f, 0.5f, 1);	// �_���[�W�󂯂����̐F
		int count = 20;			// �_�Ŏ���
		int i = 0;				// �_�ŃJ�E���g�p
		bool isAnger = false;	// �{����
		
		std::unique_ptr<Object2d> hpSprite;		// HP�\���p
		const float hpBarMaxLength = 500;		// HP�o�[�̍ő咷
		float hpBarLength;						// ����HP�o�[�̒���

		int actionIntervalTimer=200;		// ���ɍs������܂ł̎���

		// ��ԃp�^�[��
		enum class State
		{
			WAIT,		// �������Ȃ�
			MOVE,		// �ړ�
			ATTACK,		// �U��
		};
		State state;
	
		// �ړ��ʒu
		Vector3 waitPos[5] =
		{
			{  0,  6, 0},
			{-15, 14, 0},
			{ 15, 14, 0},
			{-13,  6, 0},
			{ 13,  6, 0}
		};
		int random;
		Vector3 nextPos;				// �ړ���
		Vector3 toDestinationVelocity;	// �ړ���܂ł̃x�N�g��
		Vector3 v;						// �ړ���ւ̓�������p
		bool moveUnDuplicate[5];		// �ړ���̏d�����p

		// �U�����
		enum class AttackType
		{
			SINGLESHOT,	// �P��
			RAPIDFIRE,	// �A��
			EACHSHOT,	// �g�U
		};
		AttackType attackType;
		bool attackchoseed;			// �U�����I�����t���O
		bool attacked;				// �U�������t���O
		bool attackUnDuplicate[3];	// �U���̏d�����p

		// �_�����炵�̒��S
		Vector3 center;
		// ���炵���a
		float rad = 1.0f;

		const int shotInterval = 100;	// �A�����˂̊Ԋu
		int shotIntervalTimer;			// 
		int shotTime = 3;				// ���ˉ�
		int shotCounter;				// ���ˉ񐔃J�E���g

		// �v���C���[�̕��������p
		Vector3 playerPos;
		Vector3 dir;
		Vector3 angle = { 0,0,0 };

		// �㉺���V�p
		Vector3 floatingOffsetPos;	// ���f�����㉺������p�̃I�t�Z�b�g
		float floatingOffsetWidth;	// �㉺�ړ��̕�
		float a = 0.02f;

		// �������˂�����
		int shotBallCount;

		// �{�[���̔��˒n�_�ւ̃I�t�Z�b�g
		Vector3 shotPosOffset;
		std::unique_ptr<Object3d> offsetPosObj;
		float offsetRad;
		Vector3 p, s, r;
		Vector4 c;

		float bollSpeed;		// �{�[���̃X�s�[�h

	private:
		void Move();
		void SingleShot();
		void RapidFire();
		void EachShot();
		// �v���C���[�̂ق��Ɍ���
		void SuitableForPlayer();
		// �㉺�ɂӂ�ӂ�
		void Floating();
		// �{������`�F�b�N
		void CheckIsAnger();
		// �_���[�W���o
		void DamageEffect();
		// ����̈ʒu���킹
		void ColPosSet()override;
		// ���̏����ʒu���킹
		void LegPosInitialize();

	public:
		Boss(Camera* camera, GameObjectManager* oManager);
		~Boss();
		void Initialize()override;
		void Update()override;
		void Draw()override;

		void Dead()override;
		void Delete()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		void Damage();	// �_���[�W��H�炤

		// Getter
		int GetShotBallCount() { return shotBallCount; }

	};
}