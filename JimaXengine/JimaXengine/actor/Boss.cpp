#include "Boss.h"
#include "../3d/FbxLoader.h"
#include "../general/Random.h"
#include "../general/General.h"
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
	object = std::make_unique<Object3d>(pos, scale, rotation, color);
	object->Initialize();
	object->SetModelforBuff("boss");

	pos = Vector3(0, 6, 0);
	scale = Vector3(1.5f, 1.5f, 1.5f);
	rotation = Vector3(0, 180, 0);

	Vector3 colscale = { 5,5,5 };
	aabb3dCol.maxPos = pos + colscale;
	aabb3dCol.minPos = pos - colscale;

	state = State::WAIT;
	hp = Maxhp;
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
	floatingOffsetWidth = 0.5f;

	hpSprite = std::make_unique<Object2d>();
	hpSprite->CreateSprite();

	shotBallCount = 0;

	offsetPosObj = std::make_unique<Object3d>(p, s, r, c);
	offsetPosObj->Initialize();
	offsetPosObj->SetModelforBuff("smooth_sphere");

	shotPosOffset = { 0, 0, -1 };
	offsetRad = -3.5f;
	p = pos + shotPosOffset * offsetRad;
	s = Vector3(0.3f, 0.3f, 0.3f);
	c = Vector4(1, 0, 0, 1);

	bollSpeed = 0.3f;
}

void JimaXengine::Boss::Update()
{
	Move();

	SuitableForPlayer();

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
			color = Vector4(1, 0.5f, 0.5f, 1);
		}
		else
		{
			color = Vector4(1, 1, 1, 1);
		}
	}
	else
	{
		color = Vector4(1, 1, 1, 1);
	}

	Floating();

	aabb3dCol.maxPos = Vector3(pos.x + 5, pos.y + 5, pos.z + 5);
	aabb3dCol.minPos = Vector3(pos.x - 5, pos.y - 5, pos.z - 5);

	object->SetCamera(pCamera);
	object->Update();

	offsetPosObj->SetCamera(pCamera);
	offsetPosObj->Update();
}			  

void JimaXengine::Boss::Draw()
{
	object->Draw();
	offsetPosObj->Draw();

	hpBarLength = hpBarMaxLength * hp / Maxhp;
	hpSprite->DrawOriginal("white1x1.png", Vector2(WinApp::WINDOW_WIDTH / 2 - 250, 20), 0.0f, Vector2(hpBarLength, 50.0f), "ALPHA", Vector2(), Vector4(0, 1, 0, 1));
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

	ImGui::Begin("BossInformation");
	ImGui::Text("hp : %d", hp);
	ImGui::Text("pos : %f,%f,%f", pos.x, pos.y, pos.z);
	ImGui::Text("rot : %f,%f,%f", rotation.x, rotation.y, rotation.z);
	ImGui::Text("actionIntervalTimer : %d", actionIntervalTimer);
	ImGui::Text("state : %d", state);

	ImGui::Text("playerPos : %f,%f,%f", playerPos.x, playerPos.y, playerPos.z);
	ImGui::Text("dir : %f,%f,%f", dir.x, dir.y, dir.z);
	ImGui::Text("angle : %f", angle);

	ImGui::End();
}

void JimaXengine::Boss::Damage()
{
	damaged = true;
	hp--;
	if (hp <= 0)
	{
		hp = 0;
	}
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
				nextPos = waitPos[random];
 				// ���݂̈ʒu����ړ���܂ł̑��x������
				toDestinationVelocity = (nextPos - pos);
				toDestinationVelocity = toDestinationVelocity.Normalize() / 5;
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
		// �ݒ肵���x�N�g�������Z
		pos += toDestinationVelocity;
		// ���ݒn�_�ƖڕW�n�_���r
		v = pos - nextPos;
		// ���ꂼ��̕����ŖڕW�n�_�Ƃ̍������ȉ��ɂȂ�����,�ڕW�n�_�ɌŒ肵�Ĉړ��ʂ��O�ɂ���
		if (abs(v.x) < 0.1f)
		{
			pos.x = nextPos.x;
			toDestinationVelocity.x = 0;
		}
		if (abs(v.y) < 0.1f)
		{
			pos.y = nextPos.y;
			toDestinationVelocity.y = 0;
		}
		// �ړ���ɂ�����
		if (abs(pos.x - nextPos.x)<0.5 && abs(pos.y - nextPos.y) < 0.5)
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
				//EachShot();
				SingleShot();

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
}

void JimaXengine::Boss::SingleShot()
{
	// �v���C���[�̈ʒu���Q�l�ɋ��̔��˕���������
	Vector3 targetPos = oManager->GetPlayer()->GetPos();
	// �ڕW�n�_�������_���ɂ��炷
	random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
	targetPos.x += random;
	random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
	targetPos.y += random;

	Vector3 bollVel = targetPos - (p);

	pOManager->Insert(new Target(pCamera, p, bollVel, bollSpeed));

	// �������������Z
	shotBallCount += 1;

	attacked = true;
}

void JimaXengine::Boss::RapidFire()
{
	// �v���C���[�̈ʒu���Q�l�ɋ��̔��˕���������
	Vector3 targetPos = oManager->GetPlayer()->GetPos();

	shotIntervalTimer--;
	// �s���Ԋu��0�ɂȂ�����
	if (shotIntervalTimer <= 0)
	{
		// �ڕW�n�_�������_���ɂ��炷
		random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
		targetPos.x += random;
		random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
		targetPos.y += random;

		Vector3 bollVel = targetPos - (p);

		pOManager->Insert(new Target(pCamera, p, bollVel, bollSpeed));

		// ���ˉ񐔂̉��Z
		shotCounter++;
		// ���ˊԊu�̃��Z�b�g
		shotIntervalTimer = shotInterval;

		// �������������Z
		shotBallCount += 1;
	}

	// �U���񐔂��ڕW�ɂȂ�����
	if (shotCounter >= shotTime)
	{
		shotCounter = 0;
		attacked = true;
	}
}

void JimaXengine::Boss::EachShot()
{
	// �v���C���[�̈ʒu���Q�l�ɋ��̔��˕���������
	Vector3 targetPos = oManager->GetPlayer()->GetPos();
	Vector3 bollVel = targetPos - (p);

	pOManager->Insert(new Target(pCamera, p, bollVel, bollSpeed));

	// �������������Z
	shotBallCount += 1;

	attacked = true;
}

void JimaXengine::Boss::SuitableForPlayer()
{
	// �v���C���[�̂ق�������
	playerPos = oManager->GetPlayer()->GetPos();
	dir = pos - playerPos;
	angle = { 0,0,0 };

	angle.x = atan2f(dir.y, dir.z);
	angle.y = atan2f(dir.x, dir.z);

	//float a = 0.05f;
	//if (Input::KeyPress(DIK_UP)) angle.x-=a;
	//if (Input::KeyPress(DIK_DOWN)) angle.x+=a;
	//if (Input::KeyPress(DIK_LEFT)) angle.y+=a;
	//if (Input::KeyPress(DIK_RIGHT)) angle.y-=a;

	rotation.x = angle.x * 20;
	rotation.y = 180 + (angle.y * 50);

	// �x�����W�A��
	Vector3 rotRad = Vector3::Zero;
	rotRad.x = General::ConvertToRadians(rotation.x);
	rotRad.y = General::ConvertToRadians(rotation.y);
	rotRad.z = General::ConvertToRadians(rotation.z);

	{
		// ����]�x�N�g��
		shotPosOffset = Vector3(0, 0, -1);
		
		// ��]�s������
		XMMATRIX mRot = XMMatrixRotationX(rotRad.x) * XMMatrixRotationY(rotRad.y);

		shotPosOffset = shotPosOffset * mRot;
		
		shotPosOffset *= offsetRad;

		p = pos + shotPosOffset;

		if (Input::KeyTrigger(DIK_SPACE))
		{
			SingleShot();
		}
	}
}

void JimaXengine::Boss::Floating()
{
	floatingOffsetPos.y += a;
	if (floatingOffsetPos.y < -floatingOffsetWidth || floatingOffsetPos.y > floatingOffsetWidth)
	{
		a *= -1;
	}
	pos.y += a;
}
