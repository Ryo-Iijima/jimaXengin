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
    float maxacc = 0.05f;    // �ő�����x
    float xLimit = 100.0f;    // ��ʓ��ɐ�������p
    float yLimit = 100.0f;

    // ��
    leftRacket->vel = Vector3(0, 0, 0);

    if (Input::KeyPress(DIK_W) || Input::PadLeftStickUp()) leftRacket->vel.y = defaultvel;
    if (Input::KeyPress(DIK_S) || Input::PadLeftStickDown()) leftRacket->vel.y = -defaultvel;
    if (Input::KeyPress(DIK_A) || Input::PadLeftStickLeft()) leftRacket->vel.x = -defaultvel;
    if (Input::KeyPress(DIK_D) || Input::PadLeftStickRight()) leftRacket->vel.x = defaultvel;
    leftRacket->vel.Normalize();

    leftRacket->acc += leftRacket->vel;

    // ���삳��Ă�����
    if (Input::KeyPress(DIK_W) || Input::PadLeftStickUp()
        || Input::KeyPress(DIK_S) || Input::PadLeftStickDown())
    {
        // �ō����܂łɐ����i���݂̉��������ɂ���ĕ���j
        if (leftRacket->acc.y > maxacc)
        {
            leftRacket->acc.y = maxacc;
        }
        else if (leftRacket->acc.y < -maxacc)
        {
            leftRacket->acc.y = -maxacc;
        }
    }
    // ���삳��Ă��Ȃ�������
    else
    {
        // �����i���݂̉��������ɂ���ĕ���j
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

    // ���삳��Ă�����
    if(Input::KeyPress(DIK_A) || Input::PadLeftStickLeft()
        || Input::KeyPress(DIK_D) || Input::PadLeftStickRight())
    {
        // �ō����܂łɐ����i���݂̉��������ɂ���ĕ���j
        if (leftRacket->acc.x > maxacc)
        {
            leftRacket->acc.x = maxacc;
        }
        else if (leftRacket->acc.x < -maxacc)
        {
            leftRacket->acc.x = -maxacc;
        }
    }
    // ���삳��Ă��Ȃ�������
    else
    {
        // �����i���݂̉��������ɂ���ĕ���j
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

    // ��ʊO�ɏo�Ȃ��悤��
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

    // �E
    rightRacket->vel = Vector3(0, 0, 0);
    if (Input::KeyPress(DIK_I) || Input::PadRightStickUp()) rightRacket->vel.y = defaultvel;
    if (Input::KeyPress(DIK_K) || Input::PadRightStickDown()) rightRacket->vel.y = -defaultvel;
    if (Input::KeyPress(DIK_J) || Input::PadRightStickLeft()) rightRacket->vel.x = -defaultvel;
    if (Input::KeyPress(DIK_L) || Input::PadRightStickRight()) rightRacket->vel.x = defaultvel;
    rightRacket->vel.Normalize();

    rightRacket->acc += rightRacket->vel;

    // ���삳��Ă�����
    if (Input::KeyPress(DIK_I) || Input::PadRightStickUp()
        || Input::KeyPress(DIK_K) || Input::PadRightStickDown())
    {
        // �ō����܂łɐ����i���݂̉��������ɂ���ĕ���j
        if (rightRacket->acc.y > maxacc)
        {
            rightRacket->acc.y = maxacc;
        }
        else if (rightRacket->acc.y < -maxacc)
        {
            rightRacket->acc.y = -maxacc;
        }
    }
    // ���삳��Ă��Ȃ�������
    else
    {
        // �����i���݂̉��������ɂ���ĕ���j
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

    // ���삳��Ă�����
    if (Input::KeyPress(DIK_J) || Input::PadRightStickLeft()
        || Input::KeyPress(DIK_L) || Input::PadRightStickRight())
    {
        // �ō����܂łɐ����i���݂̉��������ɂ���ĕ���j
        if (rightRacket->acc.x > maxacc)
        {
            rightRacket->acc.x = maxacc;
        }
        else if (rightRacket->acc.x < -maxacc)
        {
            rightRacket->acc.x = -maxacc;
        }
    }
    // ���삳��Ă��Ȃ�������
    else
    {
        // �����i���݂̉��������ɂ���ĕ���j
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

    // ��ʊO�ɏo�Ȃ��悤��
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
    damaged = true;
    hp--;
    if (hp <= 0)
    {
        hp = 0;
    }
}

JimaXengine::Player::Player(Camera* camera)
{
    pCamera = camera;
}

JimaXengine::Player::~Player()
{
	delete object;
	delete model;
}

void JimaXengine::Player::Initialize()
{
    object = new Object3d(pos, scale, rotation, color);
	object->Initialize();
    object->SetModelforBuff("DefaultBox");
    
    pos = Vector3(0, 1, -15);
    scale = Vector3(1, 1, 0.01f);
    color = Vector4(1, 1, 1, 0.3f);

    renderType = RENDER_TYPE::RENDER_TYPE_ALPHA_TEST;

#pragma region ���P�b�g������
    racketScale = Vector3(0.5f, 0.5f, 0.01f);

    leftRacket = std::make_unique<Racket>();

    leftRacket->pos = Vector3(-5, 2.5f, -15);

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

    for (int i = 0; i < digit; i++)
    {
        // ������������
        shotBollCountTex[i] = std::make_unique<Object2d>();
        shotBollCountTex[i]->CreateSprite();
        // �S��0�Ŗ��߂Ă���
        shotBollCountEachNum[i] = 0;

        // ������������
        hitBollCountTex[i] = std::make_unique<Object2d>();
        hitBollCountTex[i]->CreateSprite();
        // �S��0�Ŗ��߂Ă���
        hitBollCountEachNum[i] = 0;
    }

    hp = Maxhp;
    hitBollCount = 0;
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
#pragma region �����E���ۑ�
    if (Input::KeyTrigger(DIK_Z))
    {
        hitBollCount++;
    }

    // �\�����鐔���̃f�[�^���擾���Đ��񂷂�
    // �\�����������l�̌������擾
    int digitNum = General::GetDigit(hitBollCount);
    // �����Ƃɐ��l���L�^
    int result = 0;
    int numStock = hitBollCount;

    for (int i = 0; i < digitNum; i++)
    {
        // ���̂��܂肩�琔����ǂݎ��
        result = numStock % 10;
        //����10�Ŋ��������̂��g��
        numStock /= 10;

        hitBollCountEachNum[i] = result;
    }

    digitNum = General::GetDigit(oManager->GetBoss()->GetShotBallCount());
    result = 0;
    // �G���瓊���������󂯂Ƃ�
    numStock = oManager->GetBoss()->GetShotBallCount();

    for (int i = 0; i < digitNum; i++)
    {
        // ���̂��܂肩�琔����ǂݎ��
        result = numStock % 10;
        //����10�Ŋ��������̂��g��
        numStock /= 10;

        shotBollCountEachNum[i] = result;
    }

#pragma endregion �����E���ۑ�


    Move();

    object->SetCamera(pCamera);
    object->Update();

    sphereCol.center = pos.ConvertXMVECTOR();

#pragma region ���P�b�g
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
#pragma region ���P�b�g
    leftRacket->object->Draw();
    rightRacket->object->Draw();
#pragma endregion
    //object->Draw();

#pragma region �����EUI
    Vector2 uiPos = Vector2(0, WinApp::WINDOW_HEIGHT / 4 * 3);
    hpUi_0->DrawOriginal("playerUI_0.png", uiPos, 0.0f, Vector2(1.0f / 6.0f, 1.0f / 6.0f), "ALPHA");
    hpUi_1->DrawOriginal("playerUI_1.png", uiPos, 0.0f, Vector2(1.0f / 6.0f, 1.0f / 6.0f), "ALPHA");
    hpUi_2->DrawOriginal("playerUI_2.png", uiPos+Vector2(85,114.5f), 0.0f, Vector2(1.0f / 6.0f * ((float)hp / (float)Maxhp), 1.0f / 6.0f), "ALPHA");
    hpUi_3->DrawOriginal("playerUI_3.png", uiPos, 0.0f, Vector2(1.0f / 6.0f, 1.0f / 6.0f), "ALPHA");

    // �e�N�X�`���̑傫�����擾
    Vector2 texScale = Vector2((float)(Texture::GetMetadata("number.png").width) / 10, (float)(Texture::GetMetadata("number.png").height));
    Object2d::DrawTextureData d;
    d.name = "number.png";
    d.length = Vector2(1920 / 10, 270);
    d.size = texScale * 0.15f;

    for (int i = 0; i < digit; i++)
    {
                                                                                                             // ���������炷
        d.pos = Vector2(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT) * Vector2(3.5f / 20.0f, 8.0f / 10.0f) - Vector2(d.size.x * i, 0);
        // �ۑ����������̃f�[�^����؂���͈͂�����
        d.uvPos = Vector2((1920 / 10) * shotBollCountEachNum[i], 0);

        shotBollCountTex[i]->DrawRect(d);
    }

    for (int i = 0; i < digit; i++)
    {
                                                                                                             // ���������炷
        d.pos = Vector2(WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT) * Vector2(1.0f / 20.0f, 8.0f / 10.0f) - Vector2(d.size.x * i, 0);
        // �ۑ����������̃f�[�^����؂���͈͂�����
        d.uvPos = Vector2((1920 / 10) * hitBollCountEachNum[i], 0);

        hitBollCountTex[i]->DrawRect(d);
    }
#pragma endregion

#pragma region �_���[�W�G�t�F�N�g
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
    ImGui::Text("Lpos : %f,%f,%f", leftRacket->pos.x, leftRacket->pos.y, leftRacket->pos.z);
    ImGui::Text("Rpos : %f,%f,%f", rightRacket->pos.x, rightRacket->pos.y, rightRacket->pos.z);

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