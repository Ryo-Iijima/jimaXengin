#include "Target.h"
#include "../3d/FbxLoader.h"
#include "Player.h"
#include "../GameObject/GameObjectManager.h"

void Target::Move()
{
	pos.z -= 0.2f;;
	if (pos.z <= -10)
	{
		pos.z = 10;
	}

}

void Target::HitCheck()
{
	Player* player = oManager->GetPlayer();

	if (Collision::CheckSphereToSphere(sphereCol, player->GetSphereCol()))
	{
		object->SetColor(Vector4(0, 1, 0, 1));
		Dead();
	}
	else
	{
		object->SetColor(Vector4(1, 0, 0, 1));
	}
}

Target::Target(Camera* camera)
{
	pCamera = camera;
}

Target::~Target()
{
}

void Target::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("DefaultBox");
	
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);
	object->SetColor(Vector4(1, 0.3f, 0.3f, 1));


}

void Target::Update()
{
	HitCheck();
	Move();

	object->SetPosition(pos);
	object->SetCamera(pCamera);
	object->Update();

	sphereCol.center = pos.ConvertXMVECTOR();
}

void Target::Draw()
{
	object->Draw();
}

GameObject::TYPE Target::GetType()
{
	return GameObject::TYPE::TARGET;
}

void Target::DrawImGui()
{
	//ImGui::Begin("TargetInfomation");
	//ImGui::End();

}
