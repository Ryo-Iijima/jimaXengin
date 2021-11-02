#include "Target.h"
#include "../3d/FbxLoader.h"
#include "Player.h"
#include "../GameObject/GameObjectManager.h"

void JimaXengine::Target::Move()
{
	pos.z -= 0.2f;;
	if (pos.z <= -10)
	{
		pos.z = 30;
		object->SetColor(Vector4(1, 1, 1, 1));
	}

}

void JimaXengine::Target::HitCheck()
{
	Player* player = oManager->GetPlayer();

	// ƒ‰ƒPƒbƒg‚É“–‚½‚Á‚½‚ç
	if (Collision::CheckAABB3DToSphere(player->leftRacket->col, sphereCol)
		|| Collision::CheckAABB3DToSphere(player->rightRacket->col, sphereCol))
	{
		object->SetColor(Vector4(0, 0, 0, 1));
		//Dead();
	}
	else
	{
		//object->SetColor(Vector4(1, 1, 1, 1));
	}
}

JimaXengine::Target::Target(Camera* camera, Vector3 pos)
{
	pCamera = camera;
	this->pos = pos;
}

JimaXengine::Target::~Target()
{
}

void JimaXengine::Target::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("DefaultBox");
	
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);
	object->SetColor(Vector4(1, 1, 1, 1));
	object->SetScale(Vector3(1, 1, 1));
}

void JimaXengine::Target::Update()
{
	HitCheck();
	Move();

	object->SetPosition(pos);
	object->SetCamera(pCamera);
	object->Update();

	sphereCol.center = pos.ConvertXMVECTOR();
}

void JimaXengine::Target::Draw()
{
	object->Draw();
}

JimaXengine::GameObject::TYPE JimaXengine::Target::GetType()
{
	return GameObject::TYPE::TARGET;
}

void JimaXengine::Target::DrawImGui()
{
	//ImGui::Begin("TargetInfomation");
	//ImGui::End();

}
