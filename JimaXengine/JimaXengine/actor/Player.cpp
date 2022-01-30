#include "Player.h"
#include "../3d/FbxLoader.h"
#include "../general/Input.h"
#include <typeinfo>
#include "Boss.h"
#include "../general/_StringFormat.h"
#include "../GameObject/GameObjectManager.h"
#include "../general/General.h"

void JimaXengine::Player::Move()
{
#pragma region Stick
    float defaultvel = 0.05f;
    float maxacc = 0.05f;    // 最大加速度
    float xLimit = 100.0f;    // 画面内に制限する用
    float yLimit = 100.0f;

    // 左
    leftRacket->vel = Vector3(0, 0, 0);

    if (Input::KeyPress(DIK_W) || Input::PadLeftStickUp()) leftRacket->vel.y = defaultvel;
    if (Input::KeyPress(DIK_S) || Input::PadLeftStickDown()) leftRacket->vel.y = -defaultvel;
    if (Input::KeyPress(DIK_A) || Input::PadLeftStickLeft()) leftRacket->vel.x = -defaultvel;
    if (Input::KeyPress(DIK_D) || Input::PadLeftStickRight()) leftRacket->vel.x = defaultvel;
    leftRacket->vel.Normalize();

    leftRacket->acc += leftRacket->vel;

    // 操作されていたら
    if (Input::KeyPress(DIK_W) || Input::PadLeftStickUp()
        || Input::KeyPress(DIK_S) || Input::PadLeftStickDown())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (leftRacket->acc.y > maxacc)
        {
            leftRacket->acc.y = maxacc;
        }
        else if (leftRacket->acc.y < -maxacc)
        {
            leftRacket->acc.y = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (leftRacket->acc.y > 0)
        {
            leftRacket->acc.y -= defaultvel;
            if (leftRacket->acc.y <= defaultvel)
            {
                leftRacket->acc.y = 0;
            }
        }
        else if (leftRacket->acc.y < 0)
        {
            leftRacket->acc.y += defaultvel;
            if (leftRacket->acc.y >= -defaultvel)
            {
                leftRacket->acc.y = 0;
            }
        }
    }

    // 操作されていたら
    if(Input::KeyPress(DIK_A) || Input::PadLeftStickLeft()
        || Input::KeyPress(DIK_D) || Input::PadLeftStickRight())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (leftRacket->acc.x > maxacc)
        {
            leftRacket->acc.x = maxacc;
        }
        else if (leftRacket->acc.x < -maxacc)
        {
            leftRacket->acc.x = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (leftRacket->acc.x > 0)
        {
            leftRacket->acc.x -= defaultvel;
            if (leftRacket->acc.x <= defaultvel)
            {
                leftRacket->acc.x = 0;
            }
        }
        else if (leftRacket->acc.x < 0)
        {
            leftRacket->acc.x += defaultvel;
            if (leftRacket->acc.x >= -defaultvel)
            {
                leftRacket->acc.x = 0;
            }
        }
    }

    leftRacket->pos += leftRacket->acc;

    // 画面外に出ないように
    if (leftRacket->pos.x > xLimit)
    {
        leftRacket->pos.x = xLimit;
    }
    if (leftRacket->pos.x < -xLimit)
    {
        leftRacket->pos.x = -xLimit;
    }
    if (leftRacket->pos.y > yLimit)
    {
        leftRacket->pos.y = yLimit;
    }
    if (leftRacket->pos.y < -yLimit)
    {
        leftRacket->pos.y = -yLimit;
    }

    // 右
    rightRacket->vel = Vector3(0, 0, 0);
    if (Input::KeyPress(DIK_I) || Input::PadRightStickUp()) rightRacket->vel.y = defaultvel;
    if (Input::KeyPress(DIK_K) || Input::PadRightStickDown()) rightRacket->vel.y = -defaultvel;
    if (Input::KeyPress(DIK_J) || Input::PadRightStickLeft()) rightRacket->vel.x = -defaultvel;
    if (Input::KeyPress(DIK_L) || Input::PadRightStickRight()) rightRacket->vel.x = defaultvel;
    rightRacket->vel.Normalize();

    rightRacket->acc += rightRacket->vel;

    // 操作されていたら
    if (Input::KeyPress(DIK_I) || Input::PadRightStickUp()
        || Input::KeyPress(DIK_K) || Input::PadRightStickDown())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (rightRacket->acc.y > maxacc)
        {
            rightRacket->acc.y = maxacc;
        }
        else if (rightRacket->acc.y < -maxacc)
        {
            rightRacket->acc.y = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (rightRacket->acc.y > 0)
        {
            rightRacket->acc.y -= defaultvel;
            if (rightRacket->acc.y <= defaultvel)
            {
                rightRacket->acc.y = 0;
            }
        }
        else if (rightRacket->acc.y < 0)
        {
            rightRacket->acc.y += defaultvel;
            if (rightRacket->acc.y >= -defaultvel)
            {
                rightRacket->acc.y = 0;
            }
        }
    }

    // 操作されていたら
    if (Input::KeyPress(DIK_J) || Input::PadRightStickLeft()
        || Input::KeyPress(DIK_L) || Input::PadRightStickRight())
    {
        // 最高速までに制限（現在の加速方向によって分岐）
        if (rightRacket->acc.x > maxacc)
        {
            rightRacket->acc.x = maxacc;
        }
        else if (rightRacket->acc.x < -maxacc)
        {
            rightRacket->acc.x = -maxacc;
        }
    }
    // 操作されていなかったら
    else
    {
        // 減速（現在の加速方向によって分岐）
        if (rightRacket->acc.x > 0)
        {
            rightRacket->acc.x -= defaultvel;
            if (rightRacket->acc.x <= defaultvel)
            {
                rightRacket->acc.x = 0;
            }
        }
        else if (rightRacket->acc.x < 0)
        {
            rightRacket->acc.x += defaultvel;
            if (rightRacket->acc.x >= -defaultvel)
            {
                rightRacket->acc.x = 0;
            }
        }
    }

    rightRacket->pos += rightRacket->acc;

    // 画面外に出ないように
    if (rightRacket->pos.x > xLimit)
    {
        rightRacket->pos.x = xLimit;
    }
    if (rightRacket->pos.x < -xLimit)
    {
        rightRacket->pos.x = -xLimit;
    }
    if (rightRacket->pos.y > yLimit)
    {
        rightRacket->pos.y = yLimit;
    }
    if (rightRacket->pos.y < -yLimit)
    {
        rightRacket->pos.y = -yLimit;
    }

#pragma endregion
}

void JimaXengine::Player::Damage()
{
    Sound::PlayforBuff("_Player_damage.wav", 0.5f);
    damaged = true;
    hp--;
    if (hp <= 0)
    {
        hp = 0;
    }
}

void JimaXengine::Player::Swing()
{

    if ((Input::KeyTrigger(DIK_SPACE)||Input::PadButtonTrigger(XINPUT_GAMEPAD_A)) && isSwing == false)
    {
        //Sound::PlayforBuff("_Player_swing.wav");
        isSwing = true;

        character->SetAnimationFrame(150, 250, false, 2);
    }

    // 振り始めたら
    if (isSwing == true)
    {
        // カウントスタート
        swingCounter++;

        // 当たる範囲内だったら（todo:時間をx~yの間ならみたいな指定にする）
        if (swingCounter >= justHitTime &&                      // 当たり始める時間より大きく
            swingCounter < (justHitTime + justHitinterval))     // 当たり始める時間と持続時間の合計より小さい
        {
            // 当たる位置にある
            isHitZone = true;
        }
        else
        {
            // 当たる位置にない
            isHitZone = false;
        }

        bool prevIsSwung = isSwung;
        // 振り切るまでの時間が経過したら
        if (swingCounter > swingTime)
        {
            // 振り切った
            isSwung = true;
        }
        if (prevIsSwung == false && isSwung == true)
        {
            Sound::PlayforBuff("_Player_swing.wav");
        }

        // 振り切ってから構えるまでの時間が経過したら
        if (swingCounter > holdTime + swingTime)
        {
            // 構えた

            // 振っている最中ではない
            isSwing = false;
            isSwung = false;
            // カウンターリセット
            swingCounter = 0;
            //// アニメーション戻す
            //character->SetAnimationFrame(130, 149);
        }
    }
}

JimaXengine::Player::Player(Camera* camera)
{
    pCamera = camera;
}

JimaXengine::Player::~Player()
{
    delete hitBollCountTex;
    delete shotBollCountTex;
}

void JimaXengine::Player::Initialize()
{
    object = std::make_unique<Object3d>(pos, scale, rotation, color);
	object->Initialize();
    object->SetModelforBuff("DefaultBox");
    
    pos = Vector3(0, 3, -15);
    scale = Vector3(2, 2, 0.01f);
    color = Vector4(1, 1, 1, 0.3f);

    renderType = RENDER_TYPE::RENDER_TYPE_ALPHA_TEST;

#pragma region ラケット初期化
    racketScale = Vector3(0.5f, 0.5f, 0.01f);

    leftRacket = std::make_unique<Racket>();

    leftRacket->pos = Vector3(0, 2.5f, -15);

    leftRacket->object = std::make_unique<Object3d>(leftRacket->pos, leftRacket->scale, leftRacket->rot, leftRacket->color);
    leftRacket->object->Initialize();
    leftRacket->object->SetModelforBuff("AimMark");
    leftRacket->color = Vector4(1, 0.0f, 0.0f, 1);
    leftRacket->scale = racketScale;

    leftRacket->col.minPos = Vector3(leftRacket->pos - racketScale);
    leftRacket->col.maxPos = Vector3(leftRacket->pos + racketScale);


    rightRacket = std::make_unique<Racket>();

    rightRacket->pos = Vector3(5, 2.5f, -15);

    rightRacket->object = std::make_unique<Object3d>(rightRacket->pos, rightRacket->scale, rightRacket->rot, rightRacket->color);
    rightRacket->object->Initialize();
    rightRacket->object->SetModelforBuff("AimMark");
    rightRacket->color = Vector4(0.0f, 0.0f, 1, 1);
    rightRacket->scale = racketScale;

    rightRacket->col.minPos = Vector3(leftRacket->pos - racketScale);
    rightRacket->col.maxPos = Vector3(leftRacket->pos + racketScale);

#pragma endregion

    damageSprite = std::make_unique<Object2d>();
    damageSprite->CreateSprite();
    
    hpUi_0 = std::make_unique<Object2d>();
    hpUi_0->CreateSprite();
    hpUi_1 = std::make_unique<Object2d>();
    hpUi_1->CreateSprite();
    hpUi_2 = std::make_unique<Object2d>();
    hpUi_2->CreateSprite();
    hpUi_3 = std::make_unique<Object2d>();
    hpUi_3->CreateSprite();

    Object2d::DrawTextureData data;
    data.name = "number.png";
    // テクスチャの大きさを取得
    Vector2 texScale = Vector2((float)(Texture::GetMetadata(data.name).width), (float)(Texture::GetMetadata(data.name).height));
    data.length = Vector2(texScale.x / 10, texScale.y);
    data.size = data.length * 0.15f;
    data.pos = Vector2(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT) * Vector2(1.0f / 20.0f, 8.0f / 10.0f);
    data.uvPos = Vector2(data.length.x, 0);

    hitBollCountTex = new NumberDrawer(hitBollCount, 2, data);
    hitBollCountTex->Initialize();

    data.pos = Vector2(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT) * Vector2(3.5f / 20.0f, 8.0f / 10.0f);

    shotBollCountTex = new NumberDrawer(shotBollCount, 2, data);
    shotBollCountTex->Initialize();

    hp = Maxhp;
    hitBollCount = 0;
    shotBollCount = 0;

    // 打つタイミング関連
    isSwing = false;
    isHitZone = false;
    isSwung = false;
    swingCounter = 0;
    justHitTime = 15;
    justHitinterval = 10;
    swingTime = 25;
    holdTime = 20;

    character = std::make_unique<BatterCharacter>(pCamera);
    character->Initialize();

    Vector3 charaPos = pos + Vector3(5, -3, 0);
    character->SetPosition(charaPos);
    character->SetAnimationFrame(130, 149, false);
}

enum class DebugType
{
    NONE,
    ALL,
    COMMS,
    THREADING,
    IMU,
    RUMBLE,
};

void DebugPrint(std::string s, DebugType d)
{
    printf("%s\n", s.c_str());
}

void JimaXengine::Player::Update()
{    
#pragma region 数字・桁保存

    shotBollCount = oManager->GetBoss()->GetShotBallCount();

#pragma endregion 数字・桁保存

    // 移動他
    Move();

    // バットを振る
    Swing();

    // 表示キャラクター
    character->Update();

    object->SetCamera(pCamera);
    object->Update();

    sphereCol.center = pos.ConvertXMVECTOR();

#pragma region ラケット
    // left
    leftRacket->object->SetCamera(pCamera);
    leftRacket->object->Update();

    leftRacket->col.minPos = Vector3(leftRacket->pos - racketScale);
    leftRacket->col.maxPos = Vector3(leftRacket->pos + racketScale);
    // right
    rightRacket->object->SetCamera(pCamera);
    rightRacket->object->Update();

    rightRacket->col.minPos = Vector3(rightRacket->pos - racketScale);
    rightRacket->col.maxPos = Vector3(rightRacket->pos + racketScale);
#pragma endregion
}

void JimaXengine::Player::Draw()
{
#pragma region ラケット
    leftRacket->object->Draw();
    rightRacket->object->Draw();
#pragma endregion
    //object->Draw();
    character->Draw();
#pragma region 数字・UI
    //Vector2 uiPos = Vector2(0, WinApp::WINDOW_HEIGHT / 4 * 3);
    //hpUi_0->DrawOriginal("playerUI_0.png", uiPos, 0.0f, Vector2(1.0f / 6.0f, 1.0f / 6.0f), "ALPHA");
    //hpUi_1->DrawOriginal("playerUI_1.png", uiPos, 0.0f, Vector2(1.0f / 6.0f, 1.0f / 6.0f), "ALPHA");
    //hpUi_2->DrawOriginal("playerUI_2.png", uiPos+Vector2(85,114.5f), 0.0f, Vector2(1.0f / 6.0f * ((float)hp / (float)Maxhp), 1.0f / 6.0f), "ALPHA");
    //hpUi_3->DrawOriginal("playerUI_3.png", uiPos, 0.0f, Vector2(1.0f / 6.0f, 1.0f / 6.0f), "ALPHA");

    //hitBollCountTex->Draw();

    //shotBollCountTex->Draw();

#pragma endregion

#pragma region ダメージエフェクト
    if (damaged)
    {
        if (!half && damageCount > damageTime)
        {
            increase *= -1;
            half = true;
        }

        damageCount += increase;

        if (damageCount < -1)
        {
            damageCount = 0;
            increase = 1;
            damaged = false;
            half = false;
        }

        damageSprite->DrawOriginal("damagefilter.png", Vector2(0, 0), 0.0f, Vector2(1.0f / 1.5f, 1.0f / 1.5f), "ALPHA", Vector2(), Vector4(1, 0, 0, 1.0f * (float)damageCount / (float)damageTime));
    }
#pragma endregion
}

JimaXengine::GameObject::TYPE JimaXengine::Player::GetType()
{
    return GameObject::TYPE::PLAYER;
}

void JimaXengine::Player::DrawImGui()
{
    ImGui::SetNextWindowPos(ImVec2(20, 20), 1 << 1);
    ImGui::SetNextWindowSize(ImVec2(250, 300), 1 << 1);

    ImGui::Begin("PlayerInformation");
    //ImGui::Text("Lpos : %f,%f,%f", leftRacket->pos.x, leftRacket->pos.y, leftRacket->pos.z);
    //ImGui::Text("Rpos : %f,%f,%f", rightRacket->pos.x, rightRacket->pos.y, rightRacket->pos.z);
    //ImGui::Text("objePos : %f,%f,%f", pos.x, pos.y, pos.z);

    ImGui::Checkbox("isSwing", &isSwing);
    ImGui::Checkbox("isHitZone", &isHitZone);
    ImGui::Text("swingCounter : %d", swingCounter);


    ImGui::Text("CameraPos : %f,%f,%f", pCamera->GetEye().x, pCamera->GetEye().y, pCamera->GetEye().z);

    ImGui::End();
}

JimaXengine::Camera* JimaXengine::Player::GetCamera()
{
    return pCamera;
}

void JimaXengine::Player::DrawAlphaObj()
{
}