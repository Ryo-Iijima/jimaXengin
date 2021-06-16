#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#include "WinApp.h"
#include "DirectXCommon.h"

//// @drief コンソール画面にフォーマット付き文字列を表示
//// @param format フォーマット（%dとか%fとかの）
//// @param 可変長引数
//// @remarks この関数はデバッグ用です
//void DebugOutputFormatStrings(const char* format, ...) 
//{
//#ifdef _DEBUG
//	va_list valist;
//	va_start(valist, format);
//	printf(format, valist);
//#endif // _DEBUG
//}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif // _DEBUG
	//DebugOutputFormatStrings("Show window test.");
	//getchar();

	// 汎用機能
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;

	// ゲームウィンドウの作成
	winApp = new WinApp();
	winApp->CreateGameWindow();

	// DirectX初期化処理
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);


	///////////////////////////////
	// メインループ
	///////////////////////////////
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

#pragma endregion
	}
	// 各種解放処理
	delete dxCommon;

	// ゲームウィンドウの破棄
	winApp->DestroyGameWindow();
	delete winApp;

	return 0;
}
