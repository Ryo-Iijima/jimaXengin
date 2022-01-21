#pragma once
#include "GameObject/GameObject.h"

namespace JimaXengine
{
	class Camera;
	class Model;
	class Object3d;
	class Object2d;

	class OctLeg : public GameObject
	{
	private:
		std::unique_ptr<Object3d> object;
		Vector3 diffPos;		// ���S����̃Y��
		Vector3 rootPos;		// ���̍����̈ʒu

	public:
		OctLeg(Camera* camera);
		OctLeg(Camera* camera, Vector3 pos, Vector3 rot);
		~OctLeg();
		void Initialize()override;
		void Update()override;
		void Draw()override;

		// getter
		Vector3 GetDiffPos() { return diffPos; }

		// setter
		void SetPosition(Vector3 pos) { this->pos = pos; }
		void SetDiffPos(Vector3 pos) { diffPos = pos; }
		void SetRootPos(Vector3 pos) { rootPos = pos; }
		void SetRotation(Vector3 rot) { rotation = rot; }
		void SetScale(Vector3 scale) { this->scale = scale; }
	};
}

