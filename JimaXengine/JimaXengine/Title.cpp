#include "Title.h"
#include "3d/Object3d.h"
#include "3d/FbxLoader.h"
#include "Object2d.h"

Title::Title(WinApp* winapp)
{
}

Title::~Title()
{
	delete titleTex;
	delete pushStartTex;
	delete object;
	delete model;
}

void Title::Initialize()
{
	isEnd = false;
	nowScene = "Title";
	nextScene = "Play";

	model = FbxLoader::GetInstance().LoadModelFromFile("boneTest");
	// �I�u�W�F�N�g�̐����ƃ��f���̃Z�b�g
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);



	bg = new Object2d;
	bg->CreateSprite();
	bgPos = { 0,0 };
	
	
	pushStartTex = new Object2d;
	pushStartTex->CreateSprite();
	pushStartTexPos = { WinApp::WINDOW_WIDTH / 2,WinApp::WINDOW_HEIGHT / 4 * 3 };


	titleTex = new Object2d;
	titleTex->CreateSprite();
	titleTexPos = { WinApp::WINDOW_WIDTH / 2,WinApp::WINDOW_HEIGHT / 4 };
	
}

void Title::Update()
{
	object->SetPosition(Vector3(3, 3, 0));
	object->Update();

	if (Input::KeyTrigger(DIK_1))
	{

		ShutDown();
	}
}

void Title::Draw()
{
	bg->DrawOriginal("white1x1.png", bgPos, 0.0f, Vector2(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT), "ALPHA", Vector2(0.0f, 0.0f), Vector4(0.3f, 0.3f, 0.3f, 1));

	object->Draw();

	pushStartTex->DrawOriginal("pushstart.png", pushStartTexPos, 0.0f, Vector2(1.0f, 1.0f), "ALPHA", Vector2(0.5f, 0.5f));

	titleTex->DrawOriginal("title.png", titleTexPos, 0.0f, Vector2(1.0f, 1.0f), "ALPHA", Vector2(0.5f, 0.5f));

}
