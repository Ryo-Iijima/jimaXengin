#include "Boss.h"
#include "../3d/FbxLoader.h"
#include "../general/Random.h"

JimaXengine::Boss::Boss(Camera* camera)
{
	pCamera = camera;
}

JimaXengine::Boss::~Boss()
{
}

void JimaXengine::Boss::Initialize()
{
	model = FbxLoader::GetInstance().LoadModelFromFile("DefaultBox");
	object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModel(model);

	pos = Vector3(0, 0, 50);
	object->SetPosition(pos);
	object->SetScale(Vector3(10, 10, 10));

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
			object->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1));
		}
	}
	else
	{
		object->SetColor(Vector4(0.5f, 0.5f, 0.5f, 1));
	}

	object->SetPosition(pos);
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

	{
		if (Input::KeyPress(DIK_UP)) pos.y++;
		if (Input::KeyPress(DIK_DOWN)) pos.y--;
		if (Input::KeyPress(DIK_LEFT)) pos.x--;
		if (Input::KeyPress(DIK_RIGHT)) pos.x++;
	}

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
			// �ҋ@��ԂɕύX
			state = State::WAIT;
		}
		break;
	case JimaXengine::Boss::State::ATTACK:

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
