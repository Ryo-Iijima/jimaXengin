#include "Target.h"
#include "../3d/FbxLoader.h"
#include "Player.h"
#include "../GameObject/GameObjectManager.h"
#include "StrikeMark.h"

void JimaXengine::Target::Move()
{
	vel.Normalize();

	pos += (vel * speed / 2.0f) + gravity;
	rotation += {1, 1, 0};
}

void JimaXengine::Target::HitCheck()
{
	// 範囲外に出たら(playerより後ろか,bossより後ろ)
	if (pos.z > 10)
	{
		Dead();
		Delete();
	}

	// プレイヤーが跳ね返す前
	if (reflection == false)
	{
		Player* player = oManager->GetPlayer();

		// ラケットに当たったら
		if (player->GetIsHitZone()&&
			(Collision::CheckAABB3DToSphere(player->leftRacket->col, sphereCol)
			|| Collision::CheckAABB3DToSphere(player->rightRacket->col, sphereCol)))
		{
			Sound::PlayforBuff("_Player_hit.wav", 1.0f);
			locusColor = reflectLocusColor;
			reflection = true;

			// どっちのラケットに当たったか判定
			if (Collision::CheckAABB3DToSphere(player->leftRacket->col, sphereCol))
			{	
				hitRacket = (player->leftRacket.get());
			}
			else if(Collision::CheckAABB3DToSphere(player->rightRacket->col, sphereCol))
			{
				hitRacket = (player->rightRacket.get());
			}

			// ラケットの中心からどちらに寄ってるかで分岐
			Vector3 diff = hitRacket->pos - pos;
			
			vel.z *= -1;
			// 左
			if (diff.x > 0)
			{
				vel.x = -1.0f * abs(diff.x);
			}
			// 右
			else if (diff.x < 0)
			{
				vel.x = 1.0f * abs(diff.x);
			}
			// 中
			else
			{
				vel.x = 0;
			}

			// 上
			if (diff.y > 0)
			{
				vel.y = -1.0f * abs(diff.y);
			}
			// 下
			else if (diff.y < 0)
			{
				vel.y = 1.0f * abs(diff.y);
			}
			// 中
			else
			{
				vel.y = 0;
			}

		}

		// プレイヤーより後ろに行ったら
		if (pos.z <= player->GetPos().z -1)
		{
			if (isHitPlayer == false)
			{
				player->Damage();
			}

			isHitPlayer = true;

			// カメラより手前に来たら
			if (pos.z <= pCamera->GetEye().z)
			{
				Dead();
				Delete();
			}
		}
	}

	// プレイヤーが跳ね返した後
	if (reflection == true)
	{
		Boss* boss = oManager->GetBoss();

		// ボスに当たったら
		if (Collision::CheckAABB3DToSphere(boss->GetAABB3DCol(), sphereCol))
		{
			//object->SetColor(Vector4(0, 0, 0, 1));
			color = Vector4(0, 0, 0, 1);
			boss->Damage();
			Dead();
			Delete();

			oManager->GetPlayer()->AddHitBollCount();
		}
	}
}

JimaXengine::Target::Target(Camera* camera, Vector3 pos, Vector3 vel, Vector3 gool, float speed)
{
	pCamera = camera;
	this->pos = pos;
	this->vel = vel;
	this->gool = gool;
	this->speed = speed;
}

JimaXengine::Target::~Target()
{
	delete object;
}

void JimaXengine::Target::Initialize()
{	
	object = new Object3d(pos, scale, rotation, color);
	object->Initialize();
	object->SetModelforBuff("ball");
	scale = { 0.3f,0.3f, 0.3f };
	sphereCol.radius = scale.x;

	reflection = false;

	markInserted = false;

	locusColor = normalLocusColor;

	isHitPlayer = false;
}

void JimaXengine::Target::Update()
{
	if (!markInserted)
	{
		oManager->Insert(new StrikeMark(pCamera, this));
		markInserted = true;
	}

	HitCheck();
	Move();

	pEmitter->Locus(pos, locusColor);
	
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
	//ImGui::Begin("TargetInfromation");
	//ImGui::End();
}
