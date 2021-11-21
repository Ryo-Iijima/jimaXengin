#pragma once
#include "../GameObject/GameObject.h"
#include <memory>


namespace JimaXengine
{
	class Model;
	class Object3d;

	class BackGround : public GameObject
	{
	private:
		Model* model = nullptr;

		std::unique_ptr<Object3d> wall;
		std::unique_ptr<Object3d> ground;
		std::unique_ptr<Object3d> seat;

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