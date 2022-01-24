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
	
	float viewAngle = 60.0f;	// 視野角
	camera->SetProjectionMatrix(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT,viewAngle);
	
	oManager = new GameObjectManager();
	oManager->Initialize();
	oManager->Add(new Player(camera.get()));
	oManager->Add(new Boss(camera.get(),oManager));
	oManager->Add(new BackGround(camera.get()));

	isEnd = false;
	nowScene = "Play";
	nextScene = "Title";

	object2d = new Object2d();
	object2d->CreateSprite();

	lightGroup = LightGroup::Create();

	lightGroup->SetDirLightActive(0, true);
	lightGroup->SetDirLightActive(1, false);
	lightGroup->SetDirLightActive(2, false);

	lightGroup->SetPointLightActive(0, false);
	lightGroup->SetPointLightActive(1, false);
	lightGroup->SetPointLightActive(2, false);

	lightGroup->SetSpotLightActive(0, false);

	lightGroup->SetCircleShadowActiv(0, true);

	characterPos[0] = oManager->GetPlayer()->GetPos().x;
	characterPos[1] = oManager->GetPlayer()->GetPos().y;
	characterPos[2] = oManager->GetPlayer()->GetPos().z;

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
		//// ポイントライト
		//lightGroup->SetPointLightPos(0, Vector3(pointLightPos[0], pointLightPos[1], pointLightPos[2]));
		//lightGroup->SetPointLightColor(0, Vector3(pointLightColor[0], pointLightColor[1], pointLightColor[2]));
		//lightGroup->SetPointLightAtten(0, Vector3(pointLightAtten[0], pointLightAtten[1], pointLightAtten[2]));
		// スポットライト
		lightGroup->SetSpotDir(0, XMVECTOR({ spotLightDir[0], spotLightDir[1], spotLightDir[2], 0}));
		lightGroup->SetSpotLightPos(0, Vector3(spotLightPos[0], spotLightPos[1], spotLightPos[2]));
		lightGroup->SetSpotLightColor(0, Vector3(spotLightColor[0], spotLightColor[1], spotLightColor[2]));
		lightGroup->SetSpotLightAtten(0, Vector3(spotLightAtten[0], spotLightAtten[1], spotLightAtten[2]));
		lightGroup->SetSpotLightFactorAngle(0, Vector2(spotLightAngle[0], spotLightAngle[1]));
		// 丸影
		lightGroup->SetCircleShadowDir(0, XMVECTOR({ circleShadowDir[0],circleShadowDir[1], circleShadowDir[2] }));
		lightGroup->SetCircleShadowCasterPos(0, Vector3(characterPos[0], characterPos[1], characterPos[2]));
		lightGroup->SetCircleShadowAtten(0, Vector3(circleShadowAtten[0], circleShadowAtten[1], circleShadowAtten[2]));
		lightGroup->SetCircleShadowFactorAngle(0, Vector2(circleShadowFactorAngle[0], circleShadowFactorAngle[1]));

		characterPos[0] = oManager->GetPlayer()->GetPos().x;
		characterPos[1] = oManager->GetPlayer()->GetPos().y;
		characterPos[2] = oManager->GetPlayer()->GetPos().z;
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

	//ImGui::ColorEdit3("pointLightColor", pointLightColor, ImGuiColorEditFlags_Float);
	//ImGui::InputFloat3("pointLightPos", pointLightPos);
	//ImGui::InputFloat3("pointLightAtten", pointLightAtten);
	
	//ImGui::InputFloat3("spotLightDir", spotLightDir);
	//ImGui::ColorEdit3("spotLightColor", spotLightColor, ImGuiColorEditFlags_Float);
	//ImGui::InputFloat3("spotLightPos", spotLightPos);
	//ImGui::InputFloat3("spotLightAtten", spotLightAtten);
	//ImGui::InputFloat2("spotLightFactorAngle", spotLightAngle);

	ImGui::InputFloat3("circleShadowDir", circleShadowDir);
	ImGui::InputFloat3("circleShadowAtten", circleShadowAtten);
	ImGui::InputFloat2("circleShadowFactorAngle", circleShadowFactorAngle);
	ImGui::InputFloat3("characterPos", characterPos);

	ImGui::End();

}

void JimaXengine::Play::simpleStaging()
{


}
