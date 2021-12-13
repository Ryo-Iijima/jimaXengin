#include "JoyconTest.h"
#include "../general/Input.h"
#include "../imgui/imgui.h"

void JimaXengine::JoyconTest::Start()
{
    JoyconManager::Instance()->Awake();
    JoyconManager::Instance()->Start();
    SetControllers();
}


void JimaXengine::JoyconTest::OnGUI()
{
    ImGui::Begin("JoyconInfomation");
    ImGui::SetNextTreeNodeOpen(true);

    for (auto itr = m_joycons.begin(); itr != m_joycons.end(); ++itr)
    {
        bool isLeft = (*itr)->GetIsLeft();
        std::string name = isLeft ? "Joy-Con (L)" : "Joy-Con (R)";
        std::string key = isLeft ? "Z キー" : "X キー";
        Joycon::Button* button = isLeft ? m_pressedButtonL : m_pressedButtonR;
        Vector2 stick = (*itr)->GetStick();
        Vector3 gyro = (*itr)->GetGyro();
        Vector3 accel = (*itr)->GetAccel();
        Vector4 orientation = (*itr)->GetVector();
       
        if (ImGui::TreeNode("group 1")) {

            ImGui::Text("name : %s", name);
            ImGui::Text("振動 : %s", key);
            ImGui::Text("押されているボタン : %d", button);
            ImGui::Text("スティック : %d,%d", stick.x, stick.y);
            ImGui::Text("ジャイロ : %d,%d,%d", gyro.x, gyro.y, gyro.z);
            ImGui::Text("加速度 : %d,%d,%d", accel.x, accel.y, accel.z);
            ImGui::Text("傾き : %d,%d,%d,%d", orientation.x, orientation.y, orientation.z, orientation.w);
            

            ImGui::TreePop();
        }


    }

    ImGui::End();
}

void JimaXengine::JoyconTest::SetControllers()
{
     m_joycons = JoyconManager::Instance()->j;
    if (m_joycons.data() == nullptr || m_joycons.size() <= 0) return;
    for (auto itr = m_joycons.begin(); itr != m_joycons.end(); ++itr)
    {
        if ((*itr)->GetIsLeft())
        {
            m_joyconL = (*itr);
        }
        else
        {
            m_joyconR = (*itr);
        }
    }
}

JimaXengine::JoyconTest::JoyconTest()
{
}

JimaXengine::JoyconTest::~JoyconTest()
{
}

void JimaXengine::JoyconTest::Initialize()
{
    Start();
}

void JimaXengine::JoyconTest::Update()
{
    m_pressedButtonL = nullptr;
    m_pressedButtonR = nullptr;

    if (m_joycons.data() == nullptr || m_joycons.size() <= 0) return;

    SetControllers();
    //for each(var button in m_buttons)
    //{
    //    if (m_joyconL.GetButton(button))
    //    {
    //        m_pressedButtonL = button;
    //    }
    //    if (m_joyconR.GetButton(button))
    //    {
    //        m_pressedButtonR = button;
    //    }
    //}

    if (Input::KeyPress(DIK_Z))
    {
        m_joyconL->SetRumble(160, 320, 0.6f, 200);
    }
    if (Input::KeyPress(DIK_X))
    {
        m_joyconR->SetRumble(160, 320, 0.6f, 200);
    }
}

void JimaXengine::JoyconTest::Draw()
{
}

void JimaXengine::JoyconTest::Dead()
{
}

void JimaXengine::JoyconTest::Delete()
{
}

JimaXengine::GameObject::TYPE JimaXengine::JoyconTest::GetType()
{
    return TYPE();
}

JimaXengine::GameObject::RENDER_TYPE JimaXengine::JoyconTest::GetRenderType()
{
    return RENDER_TYPE();
}

void JimaXengine::JoyconTest::DrawImGui()
{
    OnGUI();
}
