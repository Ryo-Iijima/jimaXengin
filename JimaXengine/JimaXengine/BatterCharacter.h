#pragma once
#include "GameObject/GameObject.h"

namespace JimaXengine
{
	class Camera;
	class Object3d;

	class BatterCharacter : public GameObject
	{
	private:
		std::unique_ptr <Object3d> characterObject;

		std::unique_ptr <Object3d> batObject;
		Vector3 batScale;

	public:
		BatterCharacter(Camera* camera);
		~BatterCharacter();

		void Initialize()override;
		void Update()override;
		void Draw()override;
		GameObject::TYPE GetType()override;
		void DrawImGui()override;

		void SetPosition(Vector3 position) { pos = position; }
	};
}