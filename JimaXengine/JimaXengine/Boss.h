#pragma once
#include "GameObject/GameObject.h"


namespace JimaXengine
{	
	class Camera;
	class Model;
	class Object3d;

	class Boss : public GameObject
	{
	private:
		std::unique_ptr<Model> model;
		std::unique_ptr<Object3d> obj;

	public:
		Boss();
		~Boss();
		void Initialize()override;
		void Update()override;
		void Draw()override;

		void Dead()override;
		void Delete()override;
		GameObject::TYPE GetType()override;
		GameObject::RENDER_TYPE GetRenderType()override;
		void DrawImGui()override;
	};
}