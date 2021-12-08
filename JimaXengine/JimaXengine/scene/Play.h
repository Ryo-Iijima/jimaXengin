#pragma once
#include "Scene.h"
#include "../GameObject/GameObjectManager.h"
#include "../actor/JoyconTest.h"
#include "../DebugCamera.h"
#include "../Light.h"

namespace JimaXengine
{
	class Object3d;
	class Object2d;
	class Model;

	class Play : public Scene
	{
	private:
		Light* light = nullptr;

		Model* model = nullptr;

		Object2d* object2d;

		//Object3d* object3d[10000];

		std::unique_ptr<Camera> camera;

		GameObjectManager* oManager = nullptr;

		JoyconTest* jcTest = nullptr;

	private:
		
		// ŠÈˆÕ‰æ–Ê“]Š·
		void simpleStaging();

	public:
		Play(WinApp* winapp);
		~Play();
		void Initialize() override;
		void Update() override;
		void Draw() override;
	};
}