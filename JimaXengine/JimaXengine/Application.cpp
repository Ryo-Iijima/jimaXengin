#include "Application.h"
#include "3d/FbxLoader.h"
#include "3d/Object3d.h"
#include "Object2d.h"
#include "Title.h"
#include "Play.h"
#include "Texture.h"
#include "ResourceShader.h"

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

	ResourceShader::CompileShader("Sprite/SpritePixelShader.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Sprite/SpriteVertexShader.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("Geometory/ParticleVS.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("Geometory/CubePS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Geometory/CubeGS.hlsl", "main", "gs_5_0");
	ResourceShader::CompileShader("Geometory/QuadPS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Geometory/QuadGS.hlsl", "main", "gs_5_0");

	// FPS�Ǘ�
	fpsManager = new FPSManager();
	fpsManager->Initialize(60.0f);

	// �f�o�C�X�n��
	FbxLoader::GetInstance().Initialize(dxCommon->GetDevice());
	// 
	Object3d::StaticInitialize(dxCommon, winApp);
	Object3d::SetDevice(dxCommon->GetDevice());
	Object3d::CreateGraphicsPipline();
	//
	Object2d::Initialize(dxCommon, winApp);
	//
	Texture::Initialize(dxCommon);
	//
	GeometoryObject3D::PreInitialize(dxCommon);
	//
	ParticleManager::StaticInitialize();

	// ����
	input = new Input();
	input->Initialize(winApp);


	Texture::LoadTexture("white1x1.png");
	Texture::LoadTexture("colorGrid.png");
	Texture::LoadTexture("test_256x256.png");
	Texture::LoadTexture("pushstart.png");
	Texture::LoadTexture("title.png");
	Texture::LoadTexture("Stand_Tex.png");


	//object->SetInput(input);

	// �T�E���h�̓ǂݍ��݂ƍĐ�
	sound = new Sound;
	sound->Initialize();
	//sound->LoadFile("Resources/sound/Alarm01.wav");
	//sound->Play();

	// �V�[���̐ݒ�
	sceneManager = new SceneManager;
	sceneManager->Add("Title", new Title(winApp));
	sceneManager->Add("Play", new Play(winApp));

	//sceneManager->Change("Title");
	sceneManager->Change("Play");

	// imgui
	imguiDev = new ImGuiDevice();
	imguiDev->Initialize(winApp, dxCommon);
	
}

void Application::Finalize()
{
	// �e��������
	input->Finalize();

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

		fpsManager->Update();


		input->Update();

		sceneManager->Update();

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

		sceneManager->Draw();

		// �`��㏈��
		dxCommon->PostDraw();

	}
}