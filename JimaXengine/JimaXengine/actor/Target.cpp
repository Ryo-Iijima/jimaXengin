#include "Target.h"
#include "../3d/FbxLoader.h"
#include "Player.h"
#include "../GameObject/GameObjectManager.h"

void JimaXengine::Target::Move()
{
	vel.Normalize();

	pos += vel / 10;
	rotation += {1, 1, 0};
}

void JimaXengine::Target::HitCheck()
{
	// 範囲外に出たら(playerより後ろか,bossより後ろ)
	if (pos.z <= -30 || pos.z > 10)
	{
		Dead();
		Delete();
	}


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

JimaXengine::Target::Target(Camera* camera, Vector3 pos, Vector3 vel, float speed)
{
	pCamera = camera;
	this->pos = pos;
	this->vel = vel;
	this->speed = speed;
}

JimaXengine::Target::~Target()
{
}

void JimaXengine::Target::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("ball");
	
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);
	object->SetColor(Vector4(1, 1, 1, 1));
	float sca = 0.3f;
	object->SetScale(Vector3(sca, sca, sca));

	//vel = Vector3(0, 0, -0.2f);

	reflection = false;
}

void JimaXengine::Target::Update()
{
	HitCheck();
	Move();

	object->SetPosition(pos);
	object->SetRotation(rotation);
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
