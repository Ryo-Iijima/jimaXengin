#include "Application.h"
#include "3d/FbxLoader.h"
#include "3d/Object3d.h"
#include "2d/Object2d.h"
#include "scene/Title.h"
#include "scene/Play.h"
#include "scene/End.h"
#include "2d/Texture.h"
#include "ResourceShader.h"

JimaXengine::Application& JimaXengine::Application::GetInstance()
{
	static Application instance;
	return instance;
}

void JimaXengine::Application::Initialize()
{
	// �Q�[���E�B���h�E�̍쐬
	winApp = new WinApp();
	winApp->CreateGameWindow();

	// DirectX����������
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	ResourceShader::CompileShader("FBX/FBXVS.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("FBX/FBXPS.hlsl", "main", "ps_5_0");
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
	Sound::Initialize();
	//
	GeometoryObject3D::PreInitialize(dxCommon);
	//
	ParticleManager::StaticInitialize();

	// ����
	input = new Input();
	input->Initialize(winApp);
	//input->SetDeadZone(0, 0);

	Texture::LoadTexture("white1x1.png");
	Texture::LoadTexture("colorGrid.png");
	Texture::LoadTexture("test_256x256.png");

	Texture::LoadTexture("buttonfront.png");
	Texture::LoadTexture("pushstart.png");
	Texture::LoadTexture("pushbutton.png");
	Texture::LoadTexture("title.png");
	Texture::LoadTexture("damagefilter.png");
	Texture::LoadTexture("playerUI_0.png");
	Texture::LoadTexture("playerUI_1.png");
	Texture::LoadTexture("playerUI_2.png");
	Texture::LoadTexture("playerUI_3.png");
	Texture::LoadTexture("scorebord.png");
	Texture::LoadTexture("stamp_S.png");
	Texture::LoadTexture("stamp_A.png");
	Texture::LoadTexture("stamp_B.png");
	Texture::LoadTexture("stamp_C.png");
	Texture::LoadTexture("stamp_D.png");
	Texture::LoadTexture("title_bg.png");
	Texture::LoadTexture("title_bg2.png");
	Texture::LoadTexture("number.png");

	// �T�E���h�̓ǂݍ���
	Sound::LoadWav("Alarm01.wav");
	Sound::LoadWav("_title.wav");
	Sound::LoadWav("_stamp.wav");
	Sound::LoadWav("_UI_decision.wav");
	Sound::LoadWav("_UI_select.wav");
	Sound::LoadWav("_Boss_attack.wav");
	Sound::LoadWav("_Boss_damage.wav");
	Sound::LoadWav("_Boss_tentacles.wav");
	Sound::LoadWav("_Player_damage.wav");
	Sound::LoadWav("_Player_hit.wav");
	Sound::LoadWav("_Player_justmeet.wav");
	Sound::LoadWav("_Player_swing.wav");

	// ���f���̓ǂݍ���
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("DefaultPlane");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("DefaultBox");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("boss");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("wall");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("ground");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("AimMark");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("seat");
	FbxLoader::GetInstance().LoadModelFromFiletoBuff("ball");

	// �V�[���̐ݒ�
	sceneManager = new SceneManager;
	sceneManager->Add("Title", new Title(winApp));
	sceneManager->Add("Play", new Play(winApp));
	sceneManager->Add("End", new End(winApp));

	sceneManager->Change("Title");

	// imgui
	imguiDev.Initialize(winApp, dxCommon);	
}

void JimaXengine::Application::Finalize()
{
	// �e��������
	input->Finalize();

	FbxLoader::GetInstance().Finalize();

	delete dxCommon;

	// �Q�[���E�B���h�E�̔j��
	winApp->DestroyGameWindow();
	delete winApp;
}

void JimaXengine::Application::Run()
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

		imguiDev.Update();

		// �`��O����
		dxCommon->PreDraw();

		sceneManager->Draw();

		//// imgui�`��
		//{
		//	ImGui::Begin("Test Window");	// �E�B���h�E�̖��O
		//	float slider = fpsManager->GetFPS();
		//	ImGui::SliderFloat("FPS", &slider, 0.0f, 100.0f);
		//	ImGui::End();
		//}

		imguiDev.Draw();

		// �`��㏈��
		dxCommon->PostDraw();

	}
}