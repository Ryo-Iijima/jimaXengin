#pragma once
#include "../GameObject/GameObject.h"

namespace JimaXengine
{	
	class Camera;
	class Model;
	class Object3d;

	class Boss : public GameObject
	{
	private:
		Model* model;
		std::unique_ptr<Object3d> object;

		GameObjectManager* pOManager = nullptr;

		int hp;					// �q�b�g�|�C���g
		bool damaged = false;	// �_���[�W�_�ŗp
		int count = 20;			// �_�Ŏ���
		int i = 0;				// �_�ŃJ�E���g�p

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
		Vector3 waitPos[6] =
		{
			{  0,  0, 50},
			{  0,  0, 50},
			{-35, 15, 50},
			{ 35, 15, 50},
			{-35,-10, 50},
			{ 35,-10, 50}
		};
		int random;
		Vector3 nextPos;
		Vector3 toDestinationVelocity;
		Vector3 v;

		// �U�����
		enum class AttackType
		{
			SINGLESHOT,	// �P��
			RAPIDFIRE,	// �A��
			EACHSHOT,	// �g�U
		};
		AttackType attackType;
		bool attackchoseed;	// �U�����I�t���O
		bool attacked;		// �U�������t���O
		
	private:
		void Move();
		void SingleShot();
		void RapidFire();
		void EachShot();

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