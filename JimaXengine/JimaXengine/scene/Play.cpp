#include "Play.h"
#include "../3d/Object3d.h"
#include "../3d/FbxLoader.h"
#include "../2d/Object2d.h"
#include "../DebugCamera.h"

JimaXengine::Play::Play()
{
}

JimaXengine::Play::~Play()
{
	delete object2d;
	delete oManager;
	delete light;
}

void JimaXengine::Play::Initialize()
{
	Vector3 eye, target, up;

	eye = { 0, 3, -23 };
	target = { 0, 3, 10 };
	up = { 0, 1, 0 };

	camera = std::make_unique<DebugCamera>();
	camera->SetInitialViewMatrix(eye, target);
	camera->SetViewMatrix(eye, target, up);
	
	float viewAngle = 60.0f;	// Ž‹–ìŠp
	camera->SetProjectionMatrix(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT,viewAngle);
	
	oManager = new GameObjectManager();
	oManager->Initialize();
	oManager->Add(new Player(camera.get()));
	oManager->Add(new Boss(camera.get(),oManager));
	oManager->Add(new BackGround(camera.get()));
	//oManager->Add(new JoyconTest());

	//model = FbxLoader::GetInstance().LoadModelFromFile("ball");

	//for (int i = 0; i < 10000; i++)
	//{
	//	object3d[i] = new Object3d();
	//	object3d[i]->Initialize();
	//	//object3d[i]->SetModel(model);
	//	delete object3d[i];
	//}

	isEnd = false;
	nowScene = "Play";
	nextScene = "Title";

	object2d = new Object2d();
	object2d->CreateSprite();

	light = Light::Create();
	light->SetColor({ 1,1,1 });
	Object3d::SetLight(light);
}

void JimaXengine::Play::Update()
{
	camera->Move();

	{
		static XMVECTOR lightDir = { 0,-5,5,0 };
		if (Input::KeyPress(DIK_UP)) { lightDir.m128_f32[1] += 1.0f; }
		if (Input::KeyPress(DIK_DOWN)) { lightDir.m128_f32[1] -= 1.0f; }
		if (Input::KeyPress(DIK_RIGHT)) { lightDir.m128_f32[0] += 1.0f; }
		if (Input::KeyPress(DIK_LEFT)) { lightDir.m128_f32[0] -= 1.0f; }

		light->SetDir(lightDir);
	}
	light->Update();

	oManager->Update();

	if (Input::KeyTrigger(DIK_1))
	{
		ShutDown();
	}
}

void JimaXengine::Play::Draw()
{
	//object2d->DrawOriginal("colorGrid.png", Vector2(0.0f, 0.0f), 0.0f, Vector2(1.0f, 1.0f), "ALPHA");

	oManager->Draw();

}

void JimaXengine::Play::simpleStaging()
{


}
