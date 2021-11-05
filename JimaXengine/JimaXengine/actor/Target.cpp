#include "Target.h"
#include "../3d/FbxLoader.h"
#include "Player.h"
#include "../GameObject/GameObjectManager.h"

void JimaXengine::Target::Move()
{
	pos += vel;
	if (pos.z <= -10)
	{
		pos.z = 30;
		object->SetColor(Vector4(1, 1, 1, 1));
	}

}

void JimaXengine::Target::HitCheck()
{
	if (reflection == false)
	{
		Player* player = oManager->GetPlayer();

		// ラケットに当たったら
		if (Collision::CheckAABB3DToSphere(player->leftRacket->col, sphereCol)
			|| Collision::CheckAABB3DToSphere(player->rightRacket->col, sphereCol))
		{
			object->SetColor(Vector4(0, 0, 0, 1));
			reflection = true;
			vel *= -1;
		}
		else
		{
			//object->SetColor(Vector4(1, 1, 1, 1));
		}
	}

	if (reflection == true)
	{
		Boss* boss = oManager->GetBoss();

		// ボスに当たったら
		if (Collision::CheckAABB3DToSphere(boss->GetAABB3DCol(), sphereCol))
		{
			object->SetColor(Vector4(0, 0, 0, 1));
			boss->Damage();
			Dead();
		}
		else
		{
			//object->SetColor(Vector4(1, 1, 1, 1));
		}
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

	vel = Vector3(0, 0, -0.2f);

	reflection = false;
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
