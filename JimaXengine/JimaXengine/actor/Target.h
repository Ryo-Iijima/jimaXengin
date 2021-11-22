#pragma once
#include "../GameObject/GameObject.h"

namespace JimaXengine
{
	class Model;
	class Object3d;

	class Target : public GameObject
	{
	private:
		Model* model = nullptr;
		Object3d* object = nullptr;

		// ���˕Ԃ����ォ
		bool reflection = false;

	private:
		void Move();
		void HitCheck();

	public:
		Target(Camera* camera, Vector3 pos = { 0,0,0 }, Vector3 vel = { 0,0,-1 }, float speed = 0.1f);
		~Target();

		void Initialize()override;
		void Update()override;
		void Draw()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

	};
}