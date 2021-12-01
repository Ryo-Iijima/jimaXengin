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

	pos = Vector3(0, 6, 0);
	object->SetPosition(pos);
	object->SetScale(Vector3(1.5f, 1.5f, 1.5f));
	rotation = Vector3(0, 180, 0);
	object->SetRotation(rotation);

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
}

void JimaXengine::Boss::Update()
{
	Move();

	SuitableForPlayer();

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

	Floating();

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

	ImGui::Begin("BossInfomation");
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
		if (pos == nextPos)
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
	random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
	targetPos.x += random;
	random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
	targetPos.y += random;

	Vector3 bollVel = targetPos - pos;

	pOManager->Insert(new Target(pCamera, pos, bollVel));

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
		random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
		targetPos.x += random;
		random = (int)Random::GetRandom(-blurredWidth, blurredWidth);
		targetPos.y += random;

		Vector3 bollVel = targetPos - pos;

		pOManager->Insert(new Target(pCamera, pos, bollVel));

		// 発射回数の加算
		shotCounter++;
		// 発射間隔のリセット
		shotIntervalTimer = shotInterval;
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
	Vector3 bollVel = targetPos - pos;

	pOManager->Insert(new Target(pCamera, pos, bollVel));
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
	rotation.y = 180 + angle.y * 50;
}

void JimaXengine::Boss::Floating()
{
	floatingOffsetPos.y += a;
	if (floatingOffsetPos.y < -floatingOffsetWidth || floatingOffsetPos.y > floatingOffsetWidth)
	{
		a *= -1;
	}
}
