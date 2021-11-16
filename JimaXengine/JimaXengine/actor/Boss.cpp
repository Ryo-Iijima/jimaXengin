#include "Boss.h"
#include "../3d/FbxLoader.h"
#include "../general/Random.h"
#include "../GameObject/GameObjectManager.h"

JimaXengine::Boss::Boss(Camera* camera, GameObjectManager* oManager)
{
	pCamera = camera;
	pOManager = oManager;
}

JimaXengine::Boss::~Boss()
{
}

void JimaXengine::Boss::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("boss");
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModel(model);

	pos = Vector3(0, 0, 50);
	object->SetPosition(pos);
	object->SetScale(Vector3(10, 10, 10));
	rotation = Vector3(0, 180, 0);
	object->SetRotation(rotation);

	aabb3dCol.maxPos = Vector3(pos.x + 10, pos.y + 10, pos.z + 10);
	aabb3dCol.minPos = Vector3(pos.x - 10, pos.y - 10, pos.z - 10);

	state = State::WAIT;
	hp = 3;
	random = 1;
	nextPos = { 0,0,50 };
	toDestinationVelocity = { 0,0,0 };
	v = Vector3(0, 0, 0);
}

void JimaXengine::Boss::Update()
{
	Move();

	//{
	//	if (Input::KeyPress(DIK_UP)) rotation.x++;
	//	if (Input::KeyPress(DIK_DOWN)) rotation.x--;
	//	if (Input::KeyPress(DIK_LEFT)) rotation.y--;
	//	if (Input::KeyPress(DIK_RIGHT)) rotation.y++;
	//}

	// �_���[�W�󂯂Ă���_�ł���
	if (damaged)
	{
		i++;
		if (i > count)
		{
			i = 0;
			damaged = false;
		}
		if (i % 2 == 0)
		{
			object->SetColor(Vector4(1, 0.5f, 0.5f, 1));
		}
		else
		{
			object->SetColor(Vector4(1, 1, 1, 1));
		}
	}
	else
	{
		object->SetColor(Vector4(1, 1, 1, 1));
	}

	object->SetPosition(pos);
	object->SetRotation(rotation);
	aabb3dCol.maxPos = Vector3(pos.x + 5, pos.y + 5, pos.z + 5);
	aabb3dCol.minPos = Vector3(pos.x - 5, pos.y - 5, pos.z - 5);

	object->SetCamera(pCamera);
	object->Update();
}

void JimaXengine::Boss::Draw()
{
	object->Draw();
}

void JimaXengine::Boss::Dead()
{
}

void JimaXengine::Boss::Delete()
{
}

JimaXengine::GameObject::TYPE JimaXengine::Boss::GetType()
{
	return GameObject::TYPE::BOSS;
}

JimaXengine::GameObject::RENDER_TYPE JimaXengine::Boss::GetRenderType()
{
	return RENDER_TYPE();
}

void JimaXengine::Boss::DrawImGui()
{
	ImGui::SetNextWindowPos(ImVec2(920, 20), 1 << 1);
	ImGui::SetNextWindowSize(ImVec2(250, 300), 1 << 1);

	ImGui::Begin("BossInfomation");
	ImGui::Text("hp : %d", hp);
	ImGui::Text("pos : %f,%f,%f", pos.x, pos.y, pos.z);
	ImGui::Text("actionIntervalTimer : %d", actionIntervalTimer);
	ImGui::Text("state : %d", state);
	ImGui::End();
}

void JimaXengine::Boss::Damage()
{
	damaged = true;
	hp--;
}

void JimaXengine::Boss::Move()
{
	switch (state)
	{
	case JimaXengine::Boss::State::WAIT:

		actionIntervalTimer--;
		// �s���Ԋu��0�ɂȂ�����
		if (actionIntervalTimer <= 0)
		{
			// ���̈ړ���𒊑I
			random = (int)Random::GetRandom(1, 5);
			switch (random)
			{
			case 1:
				nextPos = waitPos[random];
				break;
			case 2:
				nextPos = waitPos[random];
				break;
			case 3:
				nextPos = waitPos[random];
				break;
			case 4:
				nextPos = waitPos[random];
				break;
			case 5:
				nextPos = waitPos[random];
				break;
			default:
				break;
			}
			// ���݂̈ʒu����ړ���܂ł̑��x������
			toDestinationVelocity = (nextPos - pos) / 200;
			// �ړ���ԂɕύX
			state = State::MOVE;
			// �^�C�}�[��߂�
			actionIntervalTimer = 200;
		}
		break;

	case JimaXengine::Boss::State::MOVE:

		pos += toDestinationVelocity;
		v = pos - nextPos;
		// �ړ���ɂ�����
		//if (pos == nextPos)
		if (abs(v.x) < 0.5f || abs(v.y) < 0.5f)
		{
			//// �ҋ@��ԂɕύX
			//state = State::WAIT;

			// �U����ԂɕύX
			state = State::ATTACK;
		}
		break;

	case JimaXengine::Boss::State::ATTACK:
		
		// �U���O��������
		if (!attackchoseed)
		{
			// ���̍U���𒊑I
			random = (int)Random::GetRandom(1, 1);
			attackType = (AttackType)random;

			attackchoseed = true;
		}

		if (!attacked)
		{
			switch (attackType)
			{
			case JimaXengine::Boss::AttackType::SINGLESHOT:
				SingleShot();
				break;
			case JimaXengine::Boss::AttackType::RAPIDFIRE:
				RapidFire();
				break;
			case JimaXengine::Boss::AttackType::EACHSHOT:
				EachShot();
				break;
			default:
				break;
			}
		}

		// �U������������
		if (attacked)
		{
			// �ҋ@��ԂɕύX
			state = State::WAIT;
			attackchoseed = false;
			attacked = false;
		}

		break;

	default:
		break;
	}

	float xLimit = 42.0f;    // ��ʓ��ɐ�������p
	float yLimit = 20.0f;

	// ��ʊO�ɏo�Ȃ��悤��
	if (pos.x > xLimit)
	{
		pos.x = xLimit;
	}
	if (pos.x < -xLimit)
	{
		pos.x = -xLimit;
	}
	if (pos.y > yLimit)
	{
		pos.y = yLimit;
	}
	//if (pos.y < -yLimit)
	if (pos.y < -10)
	{
		//pos.y = -yLimit;
		pos.y = -10;
	}
}

void JimaXengine::Boss::SingleShot()
{
	pOManager->Insert(new Target(pCamera, pos));

	attacked = true;
}

void JimaXengine::Boss::RapidFire()
{
	pOManager->Insert(new Target(pCamera, pos));
	attacked = true;
}

void JimaXengine::Boss::EachShot()
{
	pOManager->Insert(new Target(pCamera, pos));
	attacked = true;
}
