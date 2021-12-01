#pragma once
#include "Scene.h"
#include "../GameObject/GameObjectManager.h"
#include "../actor/JoyconTest.h"
#include "../DebugCamera.h"

class Object3d;
class Object2d;
class Model;

namespace JimaXengine
{
	class Play : public Scene
	{
	private:
		Model* model = nullptr;

		Object2d* object2d;

		//Object3d* object3d[10000];

		std::unique_ptr<Camera> camera;

		GameObjectManager* oManager = nullptr;

		JoyconTest* jcTest = nullptr;


	public:
		Play(WinApp* winapp);
		~Play();
		void Initialize() override;
		void Update() override;
		void Draw() override;

	};
}