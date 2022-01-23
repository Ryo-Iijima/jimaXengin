#include "Boss.h"
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
	object->SetModelforBuff("octotorso");
	//object->SetModelforBuff("smooth_sphere");

	pos = Vector3(0, 6, 0);
	scale *= 1.5f;
	rotation = Vector3(0, 0, 0);

	Vector3 colscale = { 5,5,5 };
	aabb3dCol.maxPos = pos + colscale;
	aabb3dCol.minPos = pos - colscale;

	legs.resize(legNum);
	for (int i = 0; i < legNum; i++)
	{
		legs[i] = std::make_unique <OctLeg>(pCamera);
		legs[i]->Initialize();
		legs[i]->SetScale(scale);
	}

	// �����ƂɈʒu�ƌ�����ݒ�
	LegPosInitialize();

	state = State::WAIT;
	hp = Maxhp;
	isAnger = false;
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

	center = Vector3(0, 3, -15);
}

void JimaXengine::Boss::Update()
{
	CheckIsAnger();

	Move();

	DamageEffect();

	SuitableForPlayer();

	Floating();

	ColPosSet();

	object->SetCamera(pCamera);
	object->Update();

	offsetPosObj->SetCamera(pCamera);
	offsetPosObj->Update();

	for (int i = 0; i < legNum; i++)
	{
		legs[i]->SetRootPos(pos);
		legs[i]->Update();
	}
}			  

void JimaXengine::Boss::Draw()
{
	object->Draw();
	offsetPosObj->Draw();

	for (int i = 0; i < legNum; i++)
	{
		legs[i]->Draw();
	}

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

	//ImGui::Text("playerPos : %f,%f,%f", playerPos.x, playerPos.y, playerPos.z);
	//ImGui::Text("dir : %f,%f,%f", dir.x, dir.y, dir.z);
	//ImGui::Text("angle : %f", angle);

	ImGui::End();
}

void JimaXengine::Boss::Damage()
{
	Sound::PlayforBuff("_Boss_damage.wav", 0.5f);
	damaged = true;
	hp--;
	if (hp <= 0)
	{
		hp = 0;
	}
}

void JimaXengine::Boss::CheckIsAnger()
{
	// �̂���HP�������ȉ��ɂȂ�����
	if (hp < (Maxhp / 2))
	{
		isAnger = true;
	}
}

void JimaXengine::Boss::DamageEffect()
{
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
			color = damageColor;
		}
		else
		{
			color = normalColor;
		}
	}
	else
	{
		color = normalColor;
	}
}

void JimaXengine::Boss::ColPosSet()
{
	aabb3dCol.maxPos = Vector3(pos.x + 5, pos.y + 5, pos.z + 5);
	aabb3dCol.minPos = Vector3(pos.x - 5, pos.y - 5, pos.z - 5);
}

void JimaXengine::Boss::LegPosInitialize()
{
	float scl = scale.x;
	legs[0]->SetDiffPos(Vector3(0.917f, 0, -0.87f) * scl);
	legs[0]->SetRotation(Vector3(0, -45, 0));

	legs[1]->SetDiffPos(Vector3(1.07f, 0, 0) * scl);
	legs[1]->SetRotation(Vector3(0, 270, 0));

	legs[2]->SetDiffPos(Vector3(1.045f, 0, 0.819f) * scl);
	legs[2]->SetRotation(Vector3(0, 230, 0));

	legs[3]->SetDiffPos(Vector3(0.358f, 0, 1.054f) * scl);
	legs[3]->SetRotation(Vector3(0, 190, 0));

	legs[4]->SetDiffPos(Vector3(-0.917f, 0, -0.87f) * scl);
	legs[4]->SetRotation(Vector3(0, 45, 0));

	legs[5]->SetDiffPos(Vector3(-1.07f, 0, 0) * scl);
	legs[5]->SetRotation(Vector3(0, -270, 0));

	legs[6]->SetDiffPos(Vector3(-1.045f, 0, 0.819f) * scl);
	legs[6]->SetRotation(Vector3(0, -230, 0));

	legs[7]->SetDiffPos(Vector3(-0.358f, 0, 1.054f) * scl);
	legs[7]->SetRotation(Vector3(0, -190, 0));
}

void JimaXengine::Boss::Move()
{
	//float defaultvel = 0.2f;
	//if (Input::KeyPress(DIK_W)) pos.y += defaultvel;
	//if (Input::KeyPress(DIK_S)) pos.y += -defaultvel;
	//if (Input::KeyPress(DIK_A)) pos.x += -defaultvel;
	//if (Input::KeyPress(DIK_D)) pos.x += defaultvel;

	switch (state)
	{
	case JimaXengine::Boss::State::WAIT:

		//actionIntervalTimer--;
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
	random = (int)Random::GetRandom(-rad, rad);
	targetPos.x += random;
	random = (int)Random::GetRandom(-rad, rad);
	targetPos.y += random;

	Vector3 bollVel = targetPos - (p);

	pOManager->Insert(new Target(pCamera, p, bollVel, targetPos, bollSpeed));

	// �������������Z
	shotBallCount += 1;

	Sound::PlayforBuff("_Boss_attack.wav", 0.5f);
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
		random = (int)Random::GetRandom(-rad, rad);
		targetPos.x += random;
		random = (int)Random::GetRandom(-rad, rad);
		targetPos.y += random;

		Vector3 bollVel = targetPos - (p);

		pOManager->Insert(new Target(pCamera, p, bollVel, targetPos, bollSpeed));

		// ���ˉ񐔂̉��Z
		shotCounter++;
		// ���ˊԊu�̃��Z�b�g
		shotIntervalTimer = shotInterval;

		// �������������Z
		shotBallCount += 1;

		Sound::PlayforBuff("_Boss_attack.wav", 0.5f);
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

	pOManager->Insert(new Target(pCamera, p, bollVel, targetPos, bollSpeed));

	// �������������Z
	shotBallCount += 1;

	Sound::PlayforBuff("_Boss_attack.wav", 0.5f);

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

	rotation.x = angle.x * 20;
	rotation.y = (angle.y * 50);

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
		// ��]�s��𔽉f
		shotPosOffset = shotPosOffset * mRot;
		// ���a���|����
		shotPosOffset *= offsetRad;
		// ���ˈʒu�ɔ��f
		p = pos + shotPosOffset;

		// �e�����̂ɍ������ĉ�]������
		LegPosInitialize();
		for (int i = 0; i < legNum; i++)
		{
			legs[i]->SetDiffPos(legs[i]->GetDiffPos() * mRot);
			legs[i]->SetRotation(legs[i]->GetRotation() + rotation);
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
