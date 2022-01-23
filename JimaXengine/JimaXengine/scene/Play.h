#pragma once
#include "Scene.h"
#include "../GameObject/GameObjectManager.h"
#include "../actor/JoyconTest.h"
#include "../DebugCamera.h"
#include "../DirectionalLight.h"

namespace JimaXengine
{
	class Object3d;
	class Object2d;
	class Model;

	class Play : public Scene
	{
	private:
		LightGroup* lightGroup = nullptr;

		Model* model = nullptr;

		Object2d* object2d;

		//Object3d* object3d[10000];

		std::unique_ptr<Camera> camera;

		GameObjectManager* oManager = nullptr;

	private:
		
		// ŠÈˆÕ‰æ–Ê“]Š·
		void simpleStaging();

		// “_ŒõŒ¹ŠÖ˜A
		float pointLightPos[3] = { 0,0,0 };
		float pointLightColor[3] = { 1,1,1 };
		float pointLightAtten[3] = { 0.3f, 0.1f, 0.1f };

	public:
		Play();
		~Play();
		void Initialize() override;
		void Update() override;
		void Draw() override;
	};
}