#include "Application.h"
#include "3d/FbxLoader.h"
#include "3d/Object3d.h"

Application& Application::GetInstance()
{
	static Application instance;
	return instance;
}

void Application::Initialize()
{
	// �Q�[���E�B���h�E�̍쐬
	winApp = new WinApp();
	winApp->CreateGameWindow();

	// DirectX����������
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	// 
	FbxLoader::GetInstance().Initialize(dxCommon->GetDevice());
	model = FbxLoader::GetInstance().LoadModelFromFile("cube");
	// 
	Object3d::SetDevice(dxCommon->GetDevice());
	Object3d::CreateGraphicsPipline();
	// �I�u�W�F�N�g�̐����ƃ��f���̃Z�b�g
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);

	// �T�E���h�̓ǂݍ��݂ƍĐ�
	sound = new Sound;
	sound->Initialize();
	//sound->LoadFile("Resources/sound/Alarm01.wav");
	//sound->Play();

	// imgui
	imguiDev = new ImGuiDevice();
	imguiDev->Initialize(winApp, dxCommon);
	
	imguiDev2 = new ImGuiDevice();
	imguiDev2->Initialize(winApp, dxCommon);

}

void Application::Finalize()
{
	// �e��������
	FbxLoader::GetInstance().Finalize();
	delete object;
	delete model;


	delete dxCommon;

	// �Q�[���E�B���h�E�̔j��
	winApp->DestroyGameWindow();
	delete winApp;
}

void Application::Run()
{
	while (true)
	{
		// ���b�Z�[�W����
		if (winApp->ProcessMessage())
		{
			break;
		}


		object->Update();

		// �`��O����
		dxCommon->PreDraw();

		//imgui�`��
		{
			imguiDev->Update();

			ImGui::Begin("Test Window");	// �E�B���h�E�̖��O

			bool blnChk = false;
			ImGui::Checkbox("CheckBoxTest", &blnChk);

			ImGui::End();

			imguiDev->Draw();
		}

		{
			imguiDev2->Update();

			ImGui::Begin("Test Window2");	// �E�B���h�E�̖��O

			bool blnChk2 = false;
			ImGui::Checkbox("CheckBoxTest", &blnChk2);


			ImGui::End();

			imguiDev2->Draw();
		}

		object->Draw(dxCommon->GetCommandList());

		// �`��㏈��
		dxCommon->PostDraw();


	}
}