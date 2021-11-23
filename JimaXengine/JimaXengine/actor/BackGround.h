#pragma once
#include "../GameObject/GameObject.h"
#include <memory>
#include "Seat.h"

namespace JimaXengine
{
	class Model;
	class Object3d;

	class BackGround : public GameObject
	{
	private:
		Model* model = nullptr;

		// ï«
		std::unique_ptr<Object3d> wall;
		Vector3 wallPos;
		Vector3 wallRot;

		// ínñ 
		std::unique_ptr<Object3d> ground;
		Vector3 groundPos;
		Vector3 groundScale;
		Vector3 groundRot;

		std::unique_ptr<Object3d> groundDart;

		// ãqê»
		std::unique_ptr<Seat> seat[15];

	public:
		BackGround(Camera* camera);
		~BackGround();

		void Initialize()override;
		void Update()override;
		void Draw()override;

		GameObject::TYPE GetType()override;
		void DrawImGui()override;

	};
}