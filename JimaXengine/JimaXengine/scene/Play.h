#pragma once
#include "Scene.h"
#include "../GameObject/GameObjectManager.h"
#include "../actor/JoyconTest.h"

class Object3d;
class Object2d;
class Model;

namespace JimaXengine
{
	class Play : public Scene
	{
	private:
		Object3d* floor;
		Model* model = nullptr;

		Object2d* object2d;

		Camera* camera = nullptr;

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