#include "Application.h"
#include "FbxLoader.h"
#include "Object3d.h"

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
	imguiDev = new imGuiDevice();
	imguiDev->Initialize(winApp, dxCommon);


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

		// imgui�`��
		imguiDev->PreDraw();
		imguiDev->Draw();

		object->Draw(dxCommon->GetCommandList());

		// �`��㏈��
		dxCommon->PostDraw();


	}
}