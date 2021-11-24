#pragma once
#include "../GameObject/GameObject.h"

namespace JimaXengine
{	
	class Camera;
	class Model;
	class Object3d;
	class Object2d;

	class Boss : public GameObject
	{
	private:
		Model* model;
		std::unique_ptr<Object3d> object;

		GameObjectManager* pOManager = nullptr;

		int hp;					// �q�b�g�|�C���g
		const int Maxhp = 3;	// �ő�q�b�g�|�C���g
		bool damaged = false;	// �_���[�W�_�ŗp
		int count = 20;			// �_�Ŏ���
		int i = 0;				// �_�ŃJ�E���g�p
		
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

		// �v���C���[�̕��������p
		Vector3 playerPos;
		Vector3 dir;
		Vector3 angle = { 0,0,0 };

		// �㉺���V�p
		Vector3 floatingOffsetPos;	// ���f�����㉺������p�̃I�t�Z�b�g
		float floatingOffsetWidth;	// �㉺�ړ��̕�
		float a = 0.02f;

	private:
		void Move();
		void SingleShot();
		void RapidFire();
		void EachShot();
		// �v���C���[�̂ق��Ɍ���
		void SuitableForPlayer();
		// �㉺�ɂӂ�ӂ�
		void Floating();

	public:
		Boss(Camera* camera, GameObjectManager* oManager);
		~Boss();
		void Initialize()override;
		void Update()override;
		void Draw()override;

		void Dead()override;
		void Delete()override;
		GameObject::TYPE GetType()override;
		GameObject::RENDER_TYPE GetRenderType()override;
		void DrawImGui()override;

		void Damage();	// �_���[�W��H�炤
	};
}