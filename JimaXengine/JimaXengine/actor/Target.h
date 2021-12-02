#pragma once
#include "../GameObject/GameObject.h"
#include "Player.h"

namespace JimaXengine
{
	class Model;
	class Object3d;
	class StrikeMark;

	class Target : public GameObject
	{
	private:
		Object3d* object = nullptr;

		// 跳ね返した後か
		bool reflection = false;

		// 当たった後の方向調整用
		Player::Racket* hitRacket = nullptr;

		bool markInserted = false;

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