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

	pos = Vector3(0, 0, 0);
	object->SetPosition(pos);
	object->SetScale(Vector3(2, 2, 2));
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

	for (int i = 0; i < 4; i++)
	{
		moveUnDuplicate[i] = false;
	}

	for (int i = 0; i < 3; i++)
	{
		attackUnDuplicate[i] = false;
	}

	floatingOffsetPos = Vector3(0, 0, 0);
	floatingOffsetWidth = 1.0f;
}

void JimaXengine::Boss::Update()
{
	Move();

	//{
	//	float a = 0.2f;
	//	if (Input::KeyPress(DIK_UP)) pos.y+=a;
	//	if (Input::KeyPress(DIK_DOWN)) pos.y-=a;
	//	if (Input::KeyPress(DIK_LEFT)) pos.x-=a;
	//	if (Input::KeyPress(DIK_RIGHT)) pos.x+=a;

	//	// �v���C���[�̂ق�������
	//	Vector3 targetPos = oManager->GetPlayer()->GetPos();
	//	Vector3 bollVel = targetPos - pos;
	//	double angle = 0;

	//	angle = atan2f(bollVel.x, bollVel.z);


	//	rotation.y = 180-angle;
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

	// �㉺�ɕ��V
	floatingOffsetPos.y += a;
	if (floatingOffsetPos.y < -floatingOffsetWidth || floatingOffsetPos.y > floatingOffsetWidth)
	{
		a *= -1;
	}

	object->SetPosition(pos + floatingOffsetPos);
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
	ImGui::Text("rot : %f,%f,%f", rotation.x, rotation.y, rotation.z);
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
			random = (int)Random::GetRandom(0, 5);

			if (!moveUnDuplicate[random])
			{
				switch (random)
				{
				case 0:
					nextPos = waitPos[random];
					break;
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
				default:
					break;
				}
				// ���݂̈ʒu����ړ���܂ł̑��x������
				toDestinationVelocity = (nextPos - pos) / 200;
				// �ړ���ԂɕύX
				state = State::MOVE;
				// �^�C�}�[��߂�
				actionIntervalTimer = 200;
				// ���̈ʒu�ɍs�������Ƃ��L�^
				moveUnDuplicate[random] = true;
				// ���ׂĂ̈ړ���Ɉړ�������
				if (moveUnDuplicate[0] && moveUnDuplicate[1] && moveUnDuplicate[2] && moveUnDuplicate[3] && moveUnDuplicate[4])
				{
					// �L�^�����Z�b�g
					for (int i = 0; i < 5; i++)
					{
						moveUnDuplicate[i] = false;
					}
				}
			}
		}
		break;

	case JimaXengine::Boss::State::MOVE:

		pos += toDestinationVelocity;
		v = pos - nextPos;
		// �ړ���ɂ�����
		if (abs(v.x) < 0.5f || abs(v.y) < 0.5f)
		{
			// �U����ԂɕύX
			state = State::ATTACK;
		}
		break;

	case JimaXengine::Boss::State::ATTACK:
		
		// �U���I��O��������
		if (!attackchoseed)
		{
			// ���̍U���𒊑I
			random = (int)Random::GetRandom(0, 3);

			// �d�����ĂȂ��U���p�^�[���Ȃ�
			if (!attackUnDuplicate[random])
			{
				// �ݒ�
				attackType = (AttackType)random;
				attackchoseed = true;
				attackUnDuplicate[random] = true;

				// ���ׂĂ̍U���p�^�[�����s������
				if (attackUnDuplicate[0] && attackUnDuplicate[1] && attackUnDuplicate[2])
				{
					for (int i = 0; i < 3; i++)
					{
						attackUnDuplicate[i] = false;
					}
				}
			}
		}

		// �U���O���U���I���ゾ������
		if (!attacked && attackchoseed)
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
	// �v���C���[�̈ʒu���Q�l�ɋ��̔��˕���������
	Vector3 targetPos = oManager->GetPlayer()->GetPos();
	Vector3 bollVel = targetPos - pos;

	pOManager->Insert(new Target(pCamera, pos, bollVel));

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
