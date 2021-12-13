#include "TestScene.h"
#include "DebugCamera.h"

JimaXengine::TestScene::TestScene()
{
}

JimaXengine::TestScene::~TestScene()
{
	delete light;
	delete oManager;
}

void JimaXengine::TestScene::Initialize()
{
	// �J����
	Vector3 eye, target, up;
	eye = { 0, 3, -23 };
	target = { 0, 3, 10 };
	up = { 0, 1, 0 };
	camera = std::make_unique<DebugCamera>();
	camera->SetInitialViewMatrix(eye, target);
	camera->SetViewMatrix(eye, target, up);

	// �I�u�W�F�N�g�}�l�[�W���[
	oManager = new GameObjectManager();
	oManager->Initialize();
	//oManager->Add(new JoyconTest());

	// ���C�g
	light = Light::Create();
	light->SetColor({ 1,1,1 });
	Object3d::SetLight(light);

	// �V�[���J��
	isEnd = false;
	nowScene = "TestScene";
	nextScene = "TestScene";
}

void JimaXengine::TestScene::Update()
{
	camera->Move();

	{
		static XMVECTOR lightDir = { 0,1,5,0 };
		if (Input::KeyPress(DIK_UP)) { lightDir.m128_f32[1] += 1.0f; }
		if (Input::KeyPress(DIK_DOWN)) { lightDir.m128_f32[1] -= 1.0f; }
		if (Input::KeyPress(DIK_RIGHT)) { lightDir.m128_f32[0] += 1.0f; }
		if (Input::KeyPress(DIK_LEFT)) { lightDir.m128_f32[0] -= 1.0f; }

		light->SetDir(lightDir);
	}
	light->Update();

	oManager->Update();
}

void JimaXengine::TestScene::Draw()
{
	oManager->Draw();
}
