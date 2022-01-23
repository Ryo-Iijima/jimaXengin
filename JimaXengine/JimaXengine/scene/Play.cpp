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
	delete lightGroup;
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

	lightGroup = LightGroup::Create();

	lightGroup->SetDirLightActive(0, false);
	lightGroup->SetDirLightActive(1, false);
	lightGroup->SetDirLightActive(2, false);

	lightGroup->SetPointLightActive(0, true);

	pointLightPos[0] =  0.5f;
	pointLightPos[1] =  1.0f;
	pointLightPos[2] =  0.0f;

	Object3d::SetLightGroup(lightGroup);
}

void JimaXengine::Play::Update()
{
	camera->Move();

	{
		//	lightGroup->SetAmbientColor(XMFLOAT3(ambientColor0));
		//	lightGroup->SetDirLightDir(0, XMVECTOR({ lightDir0[0], lightDir0[1], lightDir0[2], 0 }));
		//	lightGroup->SetDirLightColor(0, XMFLOAT3(lightColor0));
		//	lightGroup->SetDirLightDir(1, XMVECTOR({ lightDir1[0], lightDir1[1], lightDir1[2], 0 }));
		//	lightGroup->SetDirLightColor(1, XMFLOAT3(lightColor1));
		//	lightGroup->SetDirLightDir(2, XMVECTOR({ lightDir2[0], lightDir2[1], lightDir2[2], 0 }));
		//	lightGroup->SetDirLightColor(2, XMFLOAT3(lightColor2));
		lightGroup->SetPointLightPos(0, Vector3(pointLightPos[0], pointLightPos[1], pointLightPos[2]));
		lightGroup->SetPointLightColor(0, Vector3(pointLightColor[0], pointLightColor[1], pointLightColor[2]));
		lightGroup->SetPointLightAtten(0, Vector3(pointLightAtten[0], pointLightAtten[1], pointLightAtten[2]));
	}

	lightGroup->Update();

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

	ImGui::SetNextWindowPos(ImVec2(20, 20), 1 << 1);
	ImGui::SetNextWindowSize(ImVec2(250, 300), 1 << 1);

	ImGui::Begin("Light");

	ImGui::ColorEdit3("pointLightColor", pointLightColor, ImGuiColorEditFlags_Float);
	ImGui::InputFloat3("pointLightPos", pointLightPos);
	ImGui::InputFloat3("pointLightAtten", pointLightAtten);
	ImGui::End();

}

void JimaXengine::Play::simpleStaging()
{


}
