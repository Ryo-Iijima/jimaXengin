#include "Target.h"
#include "../3d/FbxLoader.h"
#include "Player.h"
#include "../GameObject/GameObjectManager.h"
#include "StrikeMark.h"

void JimaXengine::Target::Move()
{
	vel.Normalize();

	pos += vel * speed;
	rotation += {1, 1, 0};
}

void JimaXengine::Target::HitCheck()
{
	// �͈͊O�ɏo����(player����납,boss�����)
	if (pos.z > 10)
	{
		Dead();
		Delete();
	}

	// �v���C���[�����˕Ԃ��O��
	if (reflection == false)
	{
		Player* player = oManager->GetPlayer();

		// ���P�b�g�ɓ���������
		if (Collision::CheckAABB3DToSphere(player->leftRacket->col, sphereCol)
			|| Collision::CheckAABB3DToSphere(player->rightRacket->col, sphereCol))
		{
			color = Vector4(0, 1, 1, 1);
			reflection = true;

			// �ǂ����̃��P�b�g�ɓ�������������
			if (Collision::CheckAABB3DToSphere(player->leftRacket->col, sphereCol))
			{	
				hitRacket = (player->leftRacket.get());
			}
			else if(Collision::CheckAABB3DToSphere(player->rightRacket->col, sphereCol))
			{
				hitRacket = (player->rightRacket.get());
			}

			// ���P�b�g�̒��S����ǂ���Ɋ���Ă邩�ŕ���
			Vector3 diff = hitRacket->pos - pos;
			
			vel.z *= -1;
			// ��
			if (diff.x > 0)
			{
				vel.x = -1.0f * abs(diff.x);
			}
			// �E
			else if (diff.x < 0)
			{
				vel.x = 1.0f * abs(diff.x);
			}
			// ��
			else
			{
				vel.x = 0;
			}

			// ��
			if (diff.y > 0)
			{
				vel.y = -1.0f * abs(diff.y);
			}
			// ��
			else if (diff.y < 0)
			{
				vel.y = 1.0f * abs(diff.y);
			}
			// ��
			else
			{
				vel.y = 0;
			}

		}

		// �v���C���[�����ɍs������
		if (pos.z <= player->GetPos().z - 1)
		{
			player->Damage();
			Dead();
			Delete();

		}
		
	}

	// �v���C���[�����˕Ԃ�����
	if (reflection == true)
	{
		Boss* boss = oManager->GetBoss();

		// �{�X�ɓ���������
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

	//pEmitter->RandomEffect(pos);
	
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
