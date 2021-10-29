#include "Play.h"
#include "../actor/Player.h"
#include "../3d/Object3d.h"
#include "../3d/FbxLoader.h"
#include "../2d/Object2d.h"
#include "../actor/JoyconTest.h"

Play::Play(WinApp* winapp)
{
}

Play::~Play()
{
	delete oManager;
}

void Play::Initialize()
{
	Vector3 eye, target, up;

	eye = { 0, 20, -20 };
	target = { 0, 0, 0 };
	up = { 0, 1, 0 };

	camera = new Camera();
	camera->SetViewMatrix(eye, target, up);
	camera->SetProjectionMatrix(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);

	oManager = new GameObjectManager();
	oManager->Initialize();
	oManager->Add(new Player(camera));
	oManager->Add(new Target(camera));
	oManager->Add(new BackGround(camera));
	oManager->Add(new JoyconTest());

	isEnd = false;
	nowScene = "Play";
	nextScene = "Title";

	model = FbxLoader::GetInstance().LoadModelFromFile("cube");
	floor = new Object3d();
	floor->Initialize();
	floor->SetModel(model);
	floor->SetScale(Vector3(10, 0.01f, 10));

	object2d = new Object2d();
	object2d->CreateSprite();
}

void Play::Update()
{
	oManager->Update();

	floor->SetCamera(camera);
	floor->Update();

	if (Input::KeyTrigger(DIK_1))
	{
		ShutDown();
	}
}

void Play::Draw()
{
	//object2d->DrawOriginal("colorGrid.png", Vector2(0.0f, 0.0f), 0.0f, Vector2(1.0f, 1.0f), "ALPHA");

	oManager->Draw();

	//floor->Draw();
}