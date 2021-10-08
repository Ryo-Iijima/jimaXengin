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
	// ゲームウィンドウの作成
	winApp = new WinApp();
	winApp->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	ResourceShader::CompileShader("Sprite/SpritePixelShader.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Sprite/SpriteVertexShader.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("Geometory/ParticleVS.hlsl", "main", "vs_5_0");
	ResourceShader::CompileShader("Geometory/CubePS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Geometory/CubeGS.hlsl", "main", "gs_5_0");
	ResourceShader::CompileShader("Geometory/QuadPS.hlsl", "main", "ps_5_0");
	ResourceShader::CompileShader("Geometory/QuadGS.hlsl", "main", "gs_5_0");

	// FPS管理
	fpsManager = new FPSManager();
	fpsManager->Initialize(60.0f);

	// デバイス渡す
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

	// 入力
	input = new Input();
	input->Initialize(winApp);


	Texture::LoadTexture("white1x1.png");
	Texture::LoadTexture("colorGrid.png");
	Texture::LoadTexture("test_256x256.png");
	Texture::LoadTexture("pushstart.png");
	Texture::LoadTexture("title.png");
	Texture::LoadTexture("Stand_Tex.png");


	//object->SetInput(input);

	// サウンドの読み込みと再生
	sound = new Sound;
	sound->Initialize();
	//sound->LoadFile("Resources/sound/Alarm01.wav");
	//sound->Play();

	// シーンの設定
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
	// 各種解放処理
	input->Finalize();

	FbxLoader::GetInstance().Finalize();

	delete dxCommon;

	// ゲームウィンドウの破棄
	winApp->DestroyGameWindow();
	delete winApp;
}

void Application::Run()
{
	while (true)
	{
		// メッセージ処理
		if (winApp->ProcessMessage())
		{
			break;
		}

		fpsManager->Update();


		input->Update();

		sceneManager->Update();

		//imguiDev->Update();

		// 描画前処理
		dxCommon->PreDraw();

		// imgui描画
		//{
		//	ImGui::Begin("Test Window");	// ウィンドウの名前
		//	float slider = fpsManager->GetFPS();
		//	ImGui::SliderFloat("FPS", &slider, 0.0f, 100.0f);
		//	ImGui::End();

		//	imguiDev->Draw();
		//}

		sceneManager->Draw();

		// 描画後処理
		dxCommon->PostDraw();

	}
}