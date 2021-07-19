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

	// FPS�Ǘ�
	fpsManager = new FPSManager();
	fpsManager->Initialize(60.0f);

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

	// ����
	input = new Input();
	input->Initialize(winApp);

	// �T�E���h�̓ǂݍ��݂ƍĐ�
	sound = new Sound;
	sound->Initialize();
	//sound->LoadFile("Resources/sound/Alarm01.wav");
	//sound->Play();

	// imgui
	imguiDev = new ImGuiDevice();
	imguiDev->Initialize(winApp, dxCommon);
	
}

void Application::Finalize()
{
	// �e��������
	input->Finalize();

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

		fpsManager->Update();

		object->Update();

		input->Update();

		//imguiDev->Update();

		// �`��O����
		dxCommon->PreDraw();

		// imgui�`��
		//{
		//	ImGui::Begin("Test Window");	// �E�B���h�E�̖��O
		//	float slider = fpsManager->GetFPS();
		//	ImGui::SliderFloat("FPS", &slider, 0.0f, 100.0f);
		//	ImGui::End();

		//	imguiDev->Draw();
		//}

		object->Draw(dxCommon->GetCommandList());

		// �`��㏈��
		dxCommon->PostDraw();

	}
}