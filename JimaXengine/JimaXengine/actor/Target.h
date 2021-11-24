#pragma once
#include "../GameObject/GameObject.h"
#include "Player.h"

namespace JimaXengine
{
	class Model;
	class Object3d;

	class Target : public GameObject
	{
	private:
		Model* model = nullptr;
		Object3d* object = nullptr;

		// íµÇÀï‘ÇµÇΩå„Ç©
		bool reflection = false;

		// ìñÇΩÇ¡ÇΩå„ÇÃï˚å¸í≤êÆóp
		Player::Racket* hitRacket = nullptr;

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