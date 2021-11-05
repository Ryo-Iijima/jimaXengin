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
	
		// �{�X�ړ��ʒu
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
	public:
		Boss(Camera* camera);
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
		void Move();
	};
}