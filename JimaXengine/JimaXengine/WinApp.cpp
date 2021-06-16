#include "WinApp.h"

const wchar_t WinApp::windowClassName[] = L"DirectXGame";

LRESULT WinApp::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);	// OSに対して「もうこのアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	// 既定の処理を行う
}

void WinApp::CreateGameWindow()
{
	// ウィンドウクラスの設定
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WindowProcedure;	// コールバック関数の指定
	wndClass.lpszClassName = TEXT("DX12Sample");			// アプリケーションクラス名
	wndClass.hInstance = GetModuleHandle(nullptr);		// ハンドルの取得

	RegisterClassEx(&wndClass);	// アプリケーションクラス

	RECT wrc = { 0,0,WINDOW_WIDTH,WINDOW_HEIGHT };	//ウィンドウサイズを決める

	// 関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの生成
	hwnd = CreateWindow
	(
		wndClass.lpszClassName,
		TEXT("DX12テスト"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wndClass.hInstance,
		nullptr
	);

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::DestroyGameWindow()
{
	// もうクラスは使わないので登録解除する
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

bool WinApp::ProcessMessage()
{
	MSG msg = {};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// アプリケーションが終わるときにmessageがWM_QUITになる
	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}