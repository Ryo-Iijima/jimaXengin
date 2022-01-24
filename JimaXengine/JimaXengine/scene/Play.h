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
		
		// 簡易画面転換
		void simpleStaging();

		// 点光源関連
		float pointLightPos[3] = { 0,0,0 };
		float pointLightColor[3] = { 1,1,1 };
		float pointLightAtten[3] = { 0.3f, 0.1f, 0.1f };

		// スポットライト関連
		float spotLightDir[3] = { 0, -1, 0 };
		float spotLightPos[3] = { 0, 25, 0 };
		float spotLightColor[3] = { 1, 1, 1 };
		float spotLightAtten[3] = { 0.0f, 0.0f, 0.0f };
		float spotLightAngle[2] = { 20.0f, 30.0f };

		// 丸影関連
		float circleShadowDir[3] = { 0, -1, 0 };
		float circleShadowAtten[3] = { 0.5f, 0.6f, 0.0f };
		float circleShadowFactorAngle[2] = { 0.0f, 0.5f };

		float characterPos[3] = { 1, 0.0f, 0 };
		
	public:
		Play();
		~Play();
		void Initialize() override;
		void Update() override;
		void Draw() override;
	};
}