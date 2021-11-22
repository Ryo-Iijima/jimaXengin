#include "Play.h"
#include "../3d/Object3d.h"
#include "../3d/FbxLoader.h"
#include "../2d/Object2d.h"
#include "../DebugCamera.h"

JimaXengine::Play::Play(WinApp* winapp)
{
}

JimaXengine::Play::~Play()
{
	delete oManager;
}

void JimaXengine::Play::Initialize()
{
	Vector3 eye, target, up;

	eye = { 0, 0, -20 };
	target = { 0, 0, 0 };
	up = { 0, 1, 0 };

	//camera = new Camera();
	camera = std::make_unique<DebugCamera>();
	camera->SetViewMatrix(eye, target, up);

	float viewAngle = 60.0f;	// Ž‹–ìŠp
	camera->SetProjectionMatrix(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT,viewAngle);
	
	oManager = new GameObjectManager();
	oManager->Initialize();
	oManager->Add(new Player(camera.get()));
	oManager->Add(new Boss(camera.get(),oManager));
	oManager->Add(new BackGround(camera.get()));
	//oManager->Add(new JoyconTest());

	isEnd = false;
	nowScene = "Play";
	nextScene = "Title";

	object2d = new Object2d();
	object2d->CreateSprite();
}

void JimaXengine::Play::Update()
{
	camera->Move();

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
