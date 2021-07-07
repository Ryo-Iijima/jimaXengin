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
	// ゲームウィンドウの作成
	winApp = new WinApp();
	winApp->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	// 
	FbxLoader::GetInstance().Initialize(dxCommon->GetDevice());
	model = FbxLoader::GetInstance().LoadModelFromFile("cube");
	// 
	Object3d::SetDevice(dxCommon->GetDevice());
	Object3d::CreateGraphicsPipline();
	// オブジェクトの生成とモデルのセット
	object = new Object3d;
	object->Initialize();
	object->SetModel(model);

	// サウンドの読み込みと再生
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
	// 各種解放処理
	FbxLoader::GetInstance().Finalize();
	delete object;
	delete model;


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


		object->Update();

		// 描画前処理
		dxCommon->PreDraw();

		// imgui描画
		imguiDev->PreDraw();
		imguiDev->Draw();

		object->Draw(dxCommon->GetCommandList());

		// 描画後処理
		dxCommon->PostDraw();


	}
}