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

	// 足ごとに位置と向きを設定
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
	// のこりHPが半分以下になったら
	if (hp < (Maxhp / 2))
	{
		isAnger = true;
	}
}

void JimaXengine::Boss::DamageEffect()
{
	// ダメージ受けてたら点滅する
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
		// 行動間隔が0になったら
		if (actionIntervalTimer <= 0)
		{
			// 次の移動先を抽選
			random = (int)Random::GetRandom(0, 5);

			if (!moveUnDuplicate[random])
			{
				nextPos = waitPos[random];
 				// 現在の位置から移動先までの速度を決定
				toDestinationVelocity = (nextPos - pos);
				toDestinationVelocity = toDestinationVelocity.Normalize() / 5;
				// 移動状態に変更
				state = State::MOVE;
				// タイマーを戻す
				actionIntervalTimer = 200;
				// 今の位置に行ったことを記録
				moveUnDuplicate[random] = true;
				// すべての移動先に移動したら
				if (moveUnDuplicate[0] && moveUnDuplicate[1] && moveUnDuplicate[2] && moveUnDuplicate[3] && moveUnDuplicate[4])
				{
					// 記録をリセット
					for (int i = 0; i < 5; i++)
					{
						moveUnDuplicate[i] = false;
					}
				}
			}
		}
		break;

	case JimaXengine::Boss::State::MOVE:
		// 設定したベクトルを加算
		pos += toDestinationVelocity;
		// 現在地点と目標地点を比較
		v = pos - nextPos;
		// それぞれの方向で目標地点との差が一定以下になったら,目標地点に固定して移動量を０にする
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
		// 移動先についたら
		if (abs(pos.x - nextPos.x)<0.5 && abs(pos.y - nextPos.y) < 0.5)
		{
			// 攻撃状態に変更
			state = State::ATTACK;
		}
		break;

	case JimaXengine::Boss::State::ATTACK:
		
		// 攻撃選択前だったら
		if (!attackchoseed)
		{
			// 次の攻撃を抽選
			random = (int)Random::GetRandom(0, 3);

			// 重複してない攻撃パターンなら
			if (!attackUnDuplicate[random])
			{
				// 設定
				attackType = (AttackType)random;
				attackchoseed = true;
				attackUnDuplicate[random] = true;

				// すべての攻撃パターンを行ったら
				if (attackUnDuplicate[0] && attackUnDuplicate[1] && attackUnDuplicate[2])
				{
					for (int i = 0; i < 3; i++)
					{
						attackUnDuplicate[i] = false;
					}
				}
			}
		}

		// 攻撃前かつ攻撃選択後だったら
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

		// 攻撃完了したら
		if (attacked)
		{
			// 待機状態に変更
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
	// プレイヤーの位置を参考に球の発射方向を決定
	Vector3 targetPos = oManager->GetPlayer()->GetPos();
	// 目標地点をランダムにずらす
	random = (int)Random::GetRandom(-rad, rad);
	targetPos.x += random;
	random = (int)Random::GetRandom(-rad, rad);
	targetPos.y += random;

	Vector3 bollVel = targetPos - (p);

	pOManager->Insert(new Target(pCamera, p, bollVel, targetPos, bollSpeed));

	// 投げた数を加算
	shotBallCount += 1;

	Sound::PlayforBuff("_Boss_attack.wav", 0.5f);
	attacked = true;
}

void JimaXengine::Boss::RapidFire()
{
	// プレイヤーの位置を参考に球の発射方向を決定
	Vector3 targetPos = oManager->GetPlayer()->GetPos();

	shotIntervalTimer--;
	// 行動間隔が0になったら
	if (shotIntervalTimer <= 0)
	{
		// 目標地点をランダムにずらす
		random = (int)Random::GetRandom(-rad, rad);
		targetPos.x += random;
		random = (int)Random::GetRandom(-rad, rad);
		targetPos.y += random;

		Vector3 bollVel = targetPos - (p);

		pOManager->Insert(new Target(pCamera, p, bollVel, targetPos, bollSpeed));

		// 発射回数の加算
		shotCounter++;
		// 発射間隔のリセット
		shotIntervalTimer = shotInterval;

		// 投げた数を加算
		shotBallCount += 1;

		Sound::PlayforBuff("_Boss_attack.wav", 0.5f);
	}

	// 攻撃回数が目標になったら
	if (shotCounter >= shotTime)
	{
		shotCounter = 0;
		attacked = true;
	}
}

void JimaXengine::Boss::EachShot()
{
	// プレイヤーの位置を参考に球の発射方向を決定
	Vector3 targetPos = oManager->GetPlayer()->GetPos();
	Vector3 bollVel = targetPos - (p);

	pOManager->Insert(new Target(pCamera, p, bollVel, targetPos, bollSpeed));

	// 投げた数を加算
	shotBallCount += 1;

	Sound::PlayforBuff("_Boss_attack.wav", 0.5f);

	attacked = true;
}

void JimaXengine::Boss::SuitableForPlayer()
{
	// プレイヤーのほうを向く
	playerPos = oManager->GetPlayer()->GetPos();
	dir = pos - playerPos;
	angle = { 0,0,0 };

	angle.x = atan2f(dir.y, dir.z);
	angle.y = atan2f(dir.x, dir.z);

	rotation.x = angle.x * 20;
	rotation.y = (angle.y * 50);

	// 度→ラジアン
	Vector3 rotRad = Vector3::Zero;
	rotRad.x = General::ConvertToRadians(rotation.x);
	rotRad.y = General::ConvertToRadians(rotation.y);
	rotRad.z = General::ConvertToRadians(rotation.z);

	{
		// 無回転ベクトル
		shotPosOffset = Vector3(0, 0, -1);
		
		// 回転行列を作る
		XMMATRIX mRot = XMMatrixRotationX(rotRad.x) * XMMatrixRotationY(rotRad.y);
		// 回転行列を反映
		shotPosOffset = shotPosOffset * mRot;
		// 半径を掛ける
		shotPosOffset *= offsetRad;
		// 発射位置に反映
		p = pos + shotPosOffset;

		// 各足も体に合をせて回転させる
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
