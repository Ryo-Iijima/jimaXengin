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
		int count = 10;			// �_�Ŏ���
		int i = 0;				// �_�ŃJ�E���g�p

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
	};
}