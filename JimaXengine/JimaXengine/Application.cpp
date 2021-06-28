#include "Application.h"

Application& Application::Instance()
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
}

void Application::Finalize()
{
	// 各種解放処理
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

		// 描画前処理
		dxCommon->PreDraw();

		// 描画後処理
		dxCommon->PostDraw();
	}
}