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
	FbxLoader::GetInstance().LoadModelFromFile("cube");

	Object3d::SetDevice(dxCommon->GetDevice());
}

void Application::Finalize()
{
	// �e��������
	FbxLoader::GetInstance().Finalize();

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

		// �`��O����
		dxCommon->PreDraw();

		// �`��㏈��
		dxCommon->PostDraw();
	}
}