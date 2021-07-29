#include "Application.h"
#include "3d/FbxLoader.h"
#include "3d/Object3d.h"
#include "Title.h"

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

	// FPS管理
	fpsManager = new FPSManager();
	fpsManager->Initialize(60.0f);

	// デバイス渡す
	FbxLoader::GetInstance().Initialize(dxCommon->GetDevice());
	// 
	Object3d::StaticInitialize(dxCommon, winApp);
	Object3d::SetDevice(dxCommon->GetDevice());
	Object3d::CreateGraphicsPipline();

	// 入力
	input = new Input();
	input->Initialize(winApp);

	//object->SetInput(input);

	// サウンドの読み込みと再生
	sound = new Sound;
	sound->Initialize();
	//sound->LoadFile("Resources/sound/Alarm01.wav");
	//sound->Play();

	// シーンの設定
	sceneManager = new SceneManager;
	sceneManager->Add("Title", new Title(winApp));

	sceneManager->Change("Title");

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