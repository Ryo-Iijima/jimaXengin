#pragma once
#include "Scene.h"
#include "../GameObject/GameObjectManager.h"
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

		std::unique_ptr<Camera> camera;

		GameObjectManager* oManager = nullptr;

	private:
		
		// �ȈՉ�ʓ]��
		void simpleStaging();

		Vector3 ambientColor = { 1,1,1 };

		// ���s�����֘A
		float lightDir0[3] = { 0,0,0 };

		// �_�����֘A
		float pointLightPos[3][3] = { { 0,0,0 } ,{ 0,0,0 } , { 0,0,0 } };
		float pointLightColor[3][3] = { { 1,1,1 },{ 1,1,1 },{ 1,1,1 } };
		float pointLightAtten[3][3] = { { 0.3f, 0.1f, 0.1f },{ 0.3f, 0.1f, 0.1f },{ 0.3f, 0.1f, 0.1f }, };

		// �X�|�b�g���C�g�֘A
		float spotLightDir[3] = { 0, -1, 0 };
		float spotLightPos[3] = { 0, 25, 0 };
		float spotLightColor[3] = { 1, 1, 1 };
		float spotLightAtten[3] = { 0.0f, 0.0f, 0.0f };
		float spotLightAngle[2] = { 20.0f, 30.0f };

		// �t�B��
		float spotLight2Dir[3] = { 0, -1, 0 };
		float spotLight2Pos[3] = { 0, 25, 0 };

		// �o�b�N
		float spotLight3Dir[3] = { 0, -1, 0 };
		float spotLight3Pos[3] = { 0, 25, 0 };


		// �ۉe�֘A
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